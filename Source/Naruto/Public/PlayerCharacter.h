#pragma once

#include "EngineMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class NARUTO_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

#pragma region INIT
protected:
	virtual void BeginPlay() override;

public:
	APlayerCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;

#pragma endregion

private:
	/** Actor Move */
	void MoveForward(float Value);
	void MoveRight(float Value);

	/** Attach Camera To Manager */
	void AttachCamToManager();

	virtual void Jump() override;
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TSubclassOf<AActor> SpectatingViewpointClass;		//Find MainCameraManager Class...

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class AMainCameraManager* CameraManager;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class AMainPlayerController* PlayerController;

	bool isDone = false;

public:
	FORCEINLINE AMainPlayerController* GetPlayerController() {return PlayerController;}
};
