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

	/* Jump */
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Movement|Jump")
	bool bIsDoubleJump;

	UPROPERTY(EditDefaultsOnly, Category = "Movement|Jump")
	float JumpMovementForce;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerJump();
	
	FORCEINLINE void SetKeyUpDown(EKeyUpDown newKey) { Key_UD = newKey; }
	FORCEINLINE void SetKeyLeftRight(EKeyLeftRight newKey) { Key_LR = newKey; }

	FORCEINLINE EKeyUpDown GetKeyUpDown() { return Key_UD; }
	FORCEINLINE EKeyLeftRight GetKeyLeftRight() { return Key_LR; }

	/* Weapon */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AActor> StarterWeaponClass;

	UPROPERTY(Replicated, VisibleDefaultsOnly, BlueprintReadWrite,Category = "Weapon")
	class ANWeapon* CurrentWeapon;

	UPROPERTY(VisibleDefaultsOnly, Category = "Weapon")
	FName WeaponAttachSocketName;

	void SetWeapon();

	/** Attack */
	void Attack();

public:
	virtual void Jump() override;
	virtual void ResetJumpState() override;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	class UAttackActorComponent* CurAttackComp;
};