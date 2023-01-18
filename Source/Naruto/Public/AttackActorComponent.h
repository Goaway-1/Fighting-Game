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
	class ANPlayer* CurOwner;

public:
	FORCEINLINE void SetInit(ANPlayer* player) { CurOwner = player;  }

	UFUNCTION()
	void ResetAll();			// Reset Setting like Attack..!
	
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

	/** ResetOverlapActors.. */
	UFUNCTION(BlueprintCallable)
	void ClearOverlapActors();

	/** For Access from NWeapon*/
	UFUNCTION()
	FORCEINLINE int16 GetComboCnt() { return ComboCnt; }

	/** Controll ComboCnt 
	* @TODO : 오류 방지를 위한 임시방편이기에 수정필요
	*/
	UFUNCTION()
	void SetComoboCnt(int16 cnt);		

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MultiSetComoboCnt(int16 cnt);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetComboCnt(int16 cnt);
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
	TArray<AActor*> OverlapActors;	// OverlapActor's Array
#pragma endregion

#pragma region SKILL
public:
	UFUNCTION(BlueprintCallable)
	void GrapHitedCheck();				

	bool bGrapHited = false;
#pragma endregion

#pragma region ROTATE
protected:
	UPROPERTY(Replicated, VisibleAnyWhere)
	AActor* InRangeActor;	// OverlapActor's Array

public:
	UFUNCTION()
	FORCEINLINE void SetInRangeActor(AActor* Actor) { InRangeActor = Actor; }

	UFUNCTION()
	FORCEINLINE AActor* GetInRangeActor() { return InRangeActor; }

	/** Rotate to another Actor.. (Network & MutiCast) 	*/
	UFUNCTION()
	void RotateToActor();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MultiRotateToActor(FRotator Rot);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRotateToActor(FRotator Rot);

#pragma endregion

#pragma region AIRATTACK
protected:
	UPROPERTY(VisibleAnywhere, category = "Attack")
	bool bAirAttackEnd = false;			// End Current Air Attack  (현재 공중 공격이 끝났느지 여부)

	UPROPERTY(VisibleAnywhere, category = "Attack")
	bool bCanAirAttack = true;			// Can Air Attack Start? (다시 공중 공격을 할 수 있는지)
#pragma endregion

#pragma region NINJA_STAR
protected:
	UPROPERTY(EditDefaultsOnly, Category = "NinjaStar", Meta = (AllowPrivateAccess = true))
	TSubclassOf<AActor> NinjaStarClass;

	UPROPERTY(VisibleAnywhere, Category = "NinjaStar")
	class ANinjaStar* NinjaStar = nullptr;

	UPROPERTY()
	FTimerHandle NinjaStarHandle;

	UFUNCTION()
	void ResetNinjaStar();									// Reset NinjaStar
public:
	UFUNCTION()
	void ThrowNinjaStar(bool bIsChacra = false);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerThrowNinjaStar(bool bIsChacra = false);
#pragma endregion
};
