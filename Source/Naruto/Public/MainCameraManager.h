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

	//ö�� �Ǵ� ������ ��Ÿ���� ī�޶� ������ ����
	UPROPERTY(EditDefaultsOnly, Category = "Environment_Variable")
	bool bIsTekkenStyle = true;		

	//RootScene ȸ�� ���� ����
	UPROPERTY(EditDefaultsOnly, Category = "Environment_Variable")
	bool bFixRotate = false;

private:
	UPROPERTY(VisibleAnywhere, Category = "Environment_Variable")
	TArray<AActor*> Players;

	float MinRotDistance = 145.f;	//ȸ���� �ϰԵǴ� �ִ� �Ÿ� (������ ȸ��)
	float RotationDelay = 4.f;
	float Height = 25.f;

	//SpringArm�� �ּ�, �ִ� ����
	float SpringArmBaseDistance = 330.f;
	float SpringArmExtraDistance = 70.f;
	
	//SpringArm�� Pitch �ּ�, �ִ� ����
	float MinSpringArmRotation = 3.f;
	float MaxSpringArmRotation = 5.f;

	//������ �ּ�, �ִ� ���� ���� �����ϱ� ���� ���� ��
	float MaxOverlapInnerVal = 0.5f;
	float MinOverlapInnerVal = 0.07f;
	
	float IsForward;	//P1�� P2�� ���ߴµ� �յڿ� �ִ��� �� -> ��(1)/��(-1)
	float IsLeft;		//Reference���� P1�� �¿� ��� �ִ��� �� -> ����(1), ������(-1)

	float GlobalDistanceFactor;		//P1�� P2 ������ �Ÿ��� 0~1�� ������ ��ȯ
	float P1ToRoot_InnerVec;

	bool bIsPlayersOverlap = false;					//Overlap�� ����
	float OverlapRotateForce = -35.f;				//Overlap�� ȸ���� ��
	float TekkenRotateForce = 200.f;				//TekkenŸ���϶� ȸ�� ��

	void FindAndSet();
	void SetReferenceScene();			//�׻� ReferenceScene�� �׻� P1�� ���� X�࿡ �ֵ��� ����
	void RotateDefaultScene();			//�׻� ReferenceScene�� �׻� P1�� �ֺ� Y�࿡ �����ϵ��� ����
	void SetCameraPosition();			//SpringArm�� ���̿� Pitch�� �����Ͽ� ���� ���� ����
	void SetViewAllPlayers();			//�÷��̾ Overlap�� ����
	void SetNonOverlap();				//Overlap�� ī�޶� ȸ���Ͽ� ��ġ�� �ʵ��� ����
	void SetP1RelativeVal();			//P1�� P2�� ���� ������� ��ġ�� ���� (��,�� ��....)

public:
	FORCEINLINE USpringArmComponent* GetSpringArm() {return SpringArmComp;}
	FORCEINLINE UCameraComponent* GetCameraComp() { return CameraComp;}
};
