#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Actor.h"
#include "MainCameraManager.generated.h"

UCLASS()
class NARUTO_API AMainCameraManager : public APawn
{
	GENERATED_BODY()
#pragma region INIT
public:	
	AMainCameraManager();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
#pragma endregion

private:
	UPROPERTY(VisibleAnywhere, Category = "GameState")
	class ANarutoGameState* GS;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class USceneComponent* DefaultSceneRoot;			

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class USpringArmComponent* SpringArmComp;			

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UCameraComponent* CameraComp;						

	//카메라 회전과 이동의 참조를 위한 컴포넌트
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class USceneComponent* ReferenceScene;
	
	UPROPERTY(EditDefaultsOnly, Category = "Environment_Variable")
	TSubclassOf<AActor> PlayerClass;

	//철권 또는 나루토 스타일의 카메라 형식을 지정
	UPROPERTY(EditDefaultsOnly, Category = "Environment_Variable")
	bool bIsTekkenStyle = true;		

	//RootScene 회전 고정 여부
	UPROPERTY(EditDefaultsOnly, Category = "Environment_Variable")
	bool bFixRotate = false;

private:
	UPROPERTY(VisibleAnywhere, Category = "Environment_Variable")
	TArray<AActor*> Players;

	float MinRotDistance = 145.f;	//회전을 하게되는 최대 거리 (넘으면 회전)
	float RotationDelay = 4.f;
	float Height = 25.f;

	//SpringArm의 최소, 최대 길이
	float SpringArmBaseDistance = 330.f;
	float SpringArmExtraDistance = 70.f;
	
	//SpringArm의 Pitch 최소, 최대 각도
	float MinSpringArmRotation = 3.f;
	float MaxSpringArmRotation = 5.f;

	//종료의 최소, 최대 기준 값을 결정하기 위한 내적 값
	float MaxOverlapInnerVal = 0.5f;
	float MinOverlapInnerVal = 0.07f;
	
	float IsForward;	//P1이 P2와 비교했는데 앞뒤에 있는지 비교 -> 앞(1)/뒤(-1)
	float IsLeft;		//Reference기준 P1이 좌우 어디에 있는지 비교 -> 왼쪽(1), 오른쪽(-1)

	float GlobalDistanceFactor;		//P1과 P2 사이의 거리를 0~1의 비율로 변환
	float P1ToRoot_InnerVec;

	bool bIsPlayersOverlap = false;					//Overlap을 판정
	float OverlapRotateForce = -35.f;				//Overlap시 회전할 힘
	float TekkenRotateForce = 200.f;				//Tekken타입일때 회전 힘

	void FindAndSet();
	void SetReferenceScene();			//항상 ReferenceScene를 항상 P1과 같은 X축에 있도록 설정
	void RotateDefaultScene();			//항상 ReferenceScene를 항상 P1의 주변 Y축에 상주하도록 설정
	void SetCameraPosition();			//SpringArm의 길이와 Pitch값 조절하여 상하 각도 조정
	void SetViewAllPlayers();			//플레이어간 Overlap을 판정
	void SetNonOverlap();				//Overlap시 카메라를 회전하여 겹치지 않도록 수정
	void SetP1RelativeVal();			//P1가 P2에 대한 상대적인 위치를 결정 (앞,뒤 등....)

public:
	FORCEINLINE USpringArmComponent* GetSpringArm() {return SpringArmComp;}
	FORCEINLINE UCameraComponent* GetCameraComp() { return CameraComp;}
};
