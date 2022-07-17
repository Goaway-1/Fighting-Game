#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NPlayer.generated.h"

UCLASS()
class NARUTO_API ANPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	ANPlayer();
	virtual void Tick(float DeltaTime) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	/** The player moves based on the rotation of the CameraManager */
	void MoveForward(float Value);
	void MoveRight(float Value);

	/** MainCameraManager class to find */
	UPROPERTY(EditDefaultsOnly, Category = "InitSetting")
	TSubclassOf<AActor> CameraManagerClass;

	class ANCameraManager* CameraManager;

	APlayerController* PlayerControlComp;
};
