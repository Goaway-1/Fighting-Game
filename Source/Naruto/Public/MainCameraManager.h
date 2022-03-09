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
	class UStaticMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class USceneComponent* SceneComp;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class USpringArmComponent* SpringArmComp;

public:
	FORCEINLINE USpringArmComponent* GetSpringArm() {return SpringArmComp;}
	//FORCEINLINE UCameraComponent* GetCameraComp() {return CameraComp;}
};
