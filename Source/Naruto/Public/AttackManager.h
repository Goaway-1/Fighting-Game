#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttackStruct.h"
#include "AttackManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NARUTO_API UAttackManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttackManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	TWeakObjectPtr<class ANPlayer> CurOwner;
		
#pragma region ATTACK
public:
	void DefaultAttack_KeyDown(EKeyUpDown KeyUD);
	void Attack();

	UFUNCTION(BlueprintCallable)
	void EndAttack();						

	UFUNCTION(BlueprintCallable)
	void AttackInputCheck();				     // If input is in, attack the combo

	/** Get/Set Overlap Actors Method */
	UFUNCTION()
	void SetOverlapActors(AActor* actor);

	UFUNCTION()
	TArray<AActor*> GetOverlapActors();			// Return Current Overlap Actors.. 

	UFUNCTION()
	bool IsAlreadyOverlap(AActor* actor);		// Check Actor already overlap

	UFUNCTION(BlueprintCallable)
	void ClearOverlapActors();					

	/** For Access from Animation */
	FORCEINLINE int16 GetComboCnt() { return ComboCnt; }

	/** Controll ComboCnt */
	UFUNCTION()
	void SetComoboCnt(int16 cnt);		

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetComboCnt(int16 cnt);

	/** Reset Setting of Attack value */
	UFUNCTION()
	void ResetAll();					         
private:
	UPROPERTY(Replicated, VisibleAnyWhere)
	int16 ComboCnt = 1;			
	
	UPROPERTY(VisibleAnyWhere)
	bool bAttacking = false;		// is current Attacking?

	UPROPERTY(VisibleAnyWhere)
	bool bIsAttackCheck = false;	// In case of attack input during attack

	UPROPERTY(Replicated, VisibleAnyWhere)
	EKeyUpDown CurKeyUD = EKeyUpDown::EKUD_Default;			// Current UP Key Value

	UPROPERTY(Replicated, VisibleAnyWhere)
	EKeyUpDown TmpKeyUD = EKeyUpDown::EKUD_Default;			// Tmp UP Key Value

	UPROPERTY(Replicated, VisibleAnyWhere)
	TArray<AActor*> OverlapActors;	

	/** Air Attack.. */
	bool bAirAttackEnd = false;
	bool bCanAirAttack = true;

	void ResetAirAttack();
#pragma endregion

#pragma region SKILL
public:
	UFUNCTION(BlueprintCallable)
	void GrapHitedCheck();				

	bool bGrapHited = false;
#pragma endregion

#pragma region ROTATE
public:
	/** Rotate to another Actor.. (Network & MutiCast) 	*/
	UFUNCTION()
	void RotateToActor();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRotateToActor(FRotator Rot);

#pragma endregion

#pragma region NINJA_STAR
protected:
	UPROPERTY(EditDefaultsOnly, Category = "NinjaStar", Meta = (AllowPrivateAccess = true))
	TSubclassOf<AActor> NinjaStarClass;									// Default NinjaStar Class..

	UPROPERTY()
	class ANinjaStar* NinjaStar = nullptr;

	UPROPERTY()
	FTimerHandle NinjaStarHandle;

	UFUNCTION()
	void ResetNinjaStar();									

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	class USoundBase* NinjaStar_Sound;
public:
	UFUNCTION()
	void ThrowNinjaStar(bool bIsChacra = false);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerThrowNinjaStar(bool bIsChacra = false);
#pragma endregion
};
