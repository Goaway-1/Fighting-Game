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

	//ī�޶� ȸ���� �̵��� ������ ���� ������Ʈ
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class USceneComponent* ReferenceScene;
	
	UPROPERTY(EditDefaultsOnly, Category = "Environment_Variable")
	TSubclassOf<AActor> PlayerClass;

	UPROPERTY(VisibleAnywhere, Category = "Environment_Variable")
	TArray<AActor*> Players;

private:
	//ȸ���� �ϰԵǴ� �ִ� �Ÿ� (������ ȸ��)
	UPROPERTY(VisibleAnywhere, Category = "Environment_Variable")
	float MinRotDistance = 145.f;

	UPROPERTY(VisibleAnywhere, Category = "Environment_Variable")
	float RotationDelay = 4.f;

	UPROPERTY(VisibleAnywhere, Category = "Environment_Variable")
	float Height = 25.f;

	//SpringArm�� �ּ�, �ִ� ����
	UPROPERTY(VisibleAnywhere, Category = "Environment_Variable")
	float SpringArmBaseDistance = 330.f;

	UPROPERTY(VisibleAnywhere, Category = "Environment_Variable")
	float SpringArmExtraDistance = 70.f;
	
	//SpringArm�� Pitch �ּ�, �ִ� ����
	UPROPERTY(VisibleAnywhere, Category = "Environment_Variable")
	float MinSpringArmRotation = 3.f;
	
	UPROPERTY(VisibleAnywhere, Category = "Environment_Variable")
	float MaxSpringArmRotation = 5.f;
	
	//P1�� P2�� ���ߴµ� �յڿ� �ִ��� �� -> ��(1)/��(-1)
	UPROPERTY(VisibleAnywhere, Category = "Environment_Variable")
	float IsForward;	
	
	//Reference���� P1�� �¿� ��� �ִ��� �� -> ����(1), ������(-1)
	UPROPERTY(VisibleAnywhere, Category = "Environment_Variable")
	float IsLeft;

	//P1�� P2 ������ �Ÿ��� 0~1�� ������ ��ȯ
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
	void SetReferenceScene();			//�׻� ReferenceScene�� �׻� P1�� ���� X�࿡ �ֵ��� ����
	void RotateDefaultScene();
	void SetCameraPosition();

	//New
	void SetViewAllPlayers();
	void SetNonOverlap();

public:
	FORCEINLINE USpringArmComponent* GetSpringArm() {return SpringArmComp;}
	FORCEINLINE UCameraComponent* GetCameraComp() { return CameraComp;}
};
