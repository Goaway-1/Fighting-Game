#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AttackStruct.h"
#include "NPlayer.generated.h"


UCLASS()
class NARUTO_API ANPlayer : public ACharacter
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;

public:
	ANPlayer();
	virtual void Tick(float DeltaTime) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	/** MainCameraManager class to find */
	UPROPERTY(EditDefaultsOnly, Category = "InitSetting")
	TSubclassOf<AActor> CameraManagerClass;

	class ANCameraManager* CameraManager;

	APlayerController* PlayerControlComp;

#pragma region MOVE
protected:
	/** The player moves based on the rotation of the CameraManager */
	void MoveForward(float Value);
	void MoveRight(float Value);

	/* Inpressed Keys */
	UPROPERTY(VisibleAnywhere, Category = "Movement")
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

public:
	virtual void Jump() override;
	virtual void ResetJumpState() override;


#pragma endregion

#pragma region WEAPON
protected:
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

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly,  Category = "Attack")
	class UAttackActorComponent* CurAttackComp;

public:
	UFUNCTION()
	FORCEINLINE UAttackActorComponent* GetCurAttackComp() {return CurAttackComp;}

#pragma endregion

#pragma region Chacra
protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Chacra")
	class UChacraActorComponent* CurChacraComp;

	void Chacra();
public:
	UFUNCTION()
	FORCEINLINE UChacraActorComponent* GetCurChacraComp() { return CurChacraComp; }

#pragma endregion


#pragma region CHECK_ANOTHER_ACTOR
protected:
	UPROPERTY(EditAnywhere, Category = "CheckOverlap", Meta = (AllowPrivateAccess = true))
	class UCapsuleComponent* CheckOverlapActorsCollision;

public:
	UFUNCTION()
	void OnActorOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnActorOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
#pragma endregion

};