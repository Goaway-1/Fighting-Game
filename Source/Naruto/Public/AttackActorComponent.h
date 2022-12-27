#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttackStruct.h"
#include "AttackActorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NARUTO_API UAttackActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttackActorComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UPROPERTY()
	UAnimInstance* MainAnimInstance;

	UPROPERTY()
	class ANPlayer* CurOwner;

public:
	FORCEINLINE void SetInit(ANPlayer* player, UAnimInstance* AnimInst) { CurOwner = player; MainAnimInstance = AnimInst; }
	
#pragma region ATTACK
public:
	void DefaultAttack_KeyDown(EKeyUpDown KeyUD);
	void Attack();

	UFUNCTION(BlueprintCallable)
	void EndAttack();						// Reset Attack Combo...

	UFUNCTION(BlueprintCallable)
	void AttackInputCheck();				// If input is in, attack the combo

	/** Get/Set Overlap Actors Method */
	UFUNCTION()
	void SetOverlapActors(AActor* actor);

	UFUNCTION()
	TArray<AActor*> GetOverlapActors();

	UFUNCTION()
	bool IsAlreadyOverlap(AActor* actor);

	// ResetOverlapActors..
	UFUNCTION()
	void ClearOverlapActors();

	/** For Access from NWeapon*/
	UFUNCTION()
	FORCEINLINE int16 GetComboCnt() { return ComboCnt; }
private:
	FName GetAttackMontageSection(int32 Section);	// Retrun MontageNumber

	UPROPERTY(VisibleAnyWhere)
	int16 ComboCnt = 0;			
	
	UPROPERTY(VisibleAnyWhere)
	bool bAttacking = false;		// is current Attacking?

	UPROPERTY(VisibleAnyWhere)
	bool bIsAttackCheck = false;	// In case of attack input during attack

	UPROPERTY(VisibleAnyWhere)
	EKeyUpDown CurKeyUD;			// Current UP Key Value

	UPROPERTY(VisibleAnyWhere)
	EKeyUpDown TmpKeyUD;			// Tmp UP Key Value

	UPROPERTY(Replicated, VisibleAnyWhere)
	TArray<AActor*> OverlapActors;	// OverlapActor's Array
#pragma endregion

#pragma region ROTATE
protected:
	UPROPERTY(Replicated, VisibleAnyWhere)
	AActor* InRangeActor;	// OverlapActor's Array

public:
	UFUNCTION()
	FORCEINLINE void SetInRangeActor(AActor* Actor) { InRangeActor = Actor; }

	/** Rotate to another Actor.. (Network & MutiCast) 	*/
	UFUNCTION()
	void RotateToActor();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MultiRotateToActor(FRotator Rot);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRotateToActor(FRotator Rot);

#pragma endregion
#pragma region MONTAGE
public:
	void PlayNetworkMontage(UAnimMontage* Mongtage, float PlayRate, bool isSkill = false, int idx = 0);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MultiPlayNetworkMontage(UAnimMontage* Mongtage, float PlayRate, bool isSkill = false, int idx = 0);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerPlayMontage(UAnimMontage* Mongtage, float PlayRate, bool isSkill = false, int idx = 0);

	/** For Access from NWeapon */
	UFUNCTION()
	FORCEINLINE FAttackMontageStruct GetActionMontage() { return ActionMontage; }
protected:
	/** Montage List */
	UPROPERTY(EditDefaultsOnly, Category = "AttackMontage")
	FAttackMontageStruct ActionMontage;
#pragma endregion
};
