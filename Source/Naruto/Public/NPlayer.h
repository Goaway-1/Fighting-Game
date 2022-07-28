#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NPlayer.generated.h"

UENUM(BlueprintType)
enum class EKeyUpDown : uint8 {
	EKUD_Up				UMETA(DisplayName = "Up"),
	EKUD_Down			UMETA(DisplayName = "Down"),

	EKUD_Default		UMETA(DisplayName = "Default")
};

UENUM(BlueprintType)
enum class EKeyLeftRight : uint8 {
	EKLR_Right			UMETA(DisplayName = "Right"),	
	EKLR_Left			UMETA(DisplayName = "Left"),
	
	EKLR_Default		UMETA(DisplayName = "Default")
};

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

	/* Inpressed Keys */
	UPROPERTY(VisibleAnywhere, Category="Movement")
	EKeyUpDown Key_UD;

	UPROPERTY(VisibleAnywhere, Category = "Movement")
	EKeyLeftRight Key_LR;

	UPROPERTY(EditDefaultsOnly, Category = "Movement|Jump")
	float JumpMovementForce;

	/* Jump */
	UFUNCTION(Server, Reliable, WithValidation)
	void NServerJump();
	
	FORCEINLINE void SetKeyUpDown(EKeyUpDown newKey) { Key_UD = newKey; }
	FORCEINLINE void SetKeyLeftRight(EKeyLeftRight newKey) { Key_LR = newKey; }

	FORCEINLINE EKeyUpDown GetKeyUpDown() { return Key_UD; }
	FORCEINLINE EKeyLeftRight GetKeyLeftRight() { return Key_LR; }

	/* Weapon */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AActor> StarterWeaponClass;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	class ANWeapon* CurrentWeapon;

	UPROPERTY(VisibleDefaultsOnly, Category = "Weapon")
	FName WeaponAttachSocketName;
public:
	void NJump();
	virtual void Jump() override;
	virtual void ResetJumpState() override;
};