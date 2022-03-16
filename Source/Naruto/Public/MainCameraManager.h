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

	UPROPERTY(VisibleAnywhere, Category = "Environment_Variable")
	TArray<AActor*> Players;

private:
	//회전을 하게되는 최대 거리 (넘으면 회전)
	UPROPERTY(VisibleAnywhere, Category = "Environment_Variable")
	float MinRotDistance = 145.f;

	UPROPERTY(VisibleAnywhere, Category = "Environment_Variable")
	float RotationDelay = 4.f;

	UPROPERTY(VisibleAnywhere, Category = "Environment_Variable")
	float Height = 25.f;

	//SpringArm의 최소, 최대 길이
	UPROPERTY(VisibleAnywhere, Category = "Environment_Variable")
	float SpringArmBaseDistance = 330.f;

	UPROPERTY(VisibleAnywhere, Category = "Environment_Variable")
	float SpringArmExtraDistance = 70.f;
	
	//SpringArm의 Pitch 최소, 최대 각도
	UPROPERTY(VisibleAnywhere, Category = "Environment_Variable")
	float MinSpringArmRotation = 3.f;
	
	UPROPERTY(VisibleAnywhere, Category = "Environment_Variable")
	float MaxSpringArmRotation = 5.f;
	
	//P1이 P2와 비교했는데 앞뒤에 있는지 비교 -> 앞(1)/뒤(-1)
	UPROPERTY(VisibleAnywhere, Category = "Environment_Variable")
	float IsForward;	
	
	//Reference기준 P1이 좌우 어디에 있는지 비교 -> 왼쪽(1), 오른쪽(-1)
	UPROPERTY(VisibleAnywhere, Category = "Environment_Variable")
	float IsLeft;

	//P1과 P2 사이의 거리를 0~1의 비율로 변환
	UPROPERTY(VisibleAnywhere, Category = "Environment_Variable")
	float GlobalDistanceFactor;

	//New
	UPROPERTY(VisibleAnywhere, Category = "Environment_Variable")
	float P1ToRoot_InnerVec;

	float OverlapForce = -15.f;
	bool bIsPlayersOverlap = false;
	float MinInnerVal = 0.07;
	float MaxOverlapInnerVal = 0.3f;
	float MinOverlapInnerVal = 0.05f;


	void FindAndSet();
	void SetReferenceScene();			//항상 ReferenceScene를 항상 P1과 같은 X축에 있도록 설정
	void RotateDefaultScene();
	void SetCameraPosition();

	//New
	void SetViewAllPlayers();
	void SetNonOverlap();

public:
	FORCEINLINE USpringArmComponent* GetSpringArm() {return SpringArmComp;}
	FORCEINLINE UCameraComponent* GetCameraComp() { return CameraComp;}
};
