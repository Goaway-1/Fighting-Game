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

public:
	/** 이거 내일 정리하기 */
	void DefaultAttack_KeyDown();
	void Attack();	

	UFUNCTION(BlueprintCallable)
	void EndAttack();

	UFUNCTION(BlueprintCallable)
	void AttackInputCheck();

	FName GetAttackMontageSection(int32 Section);

	int16 ComboCnt = 0;
	bool bAttacking = false;
	bool bLMBDown = false;
	bool bIsAttackCheck = false;

#pragma region MONTAGE
public:
	/** Montage */
	FORCEINLINE void SetAnimInstance(UAnimInstance* AnimInst) { MainAnimInstance = AnimInst; }

	void PlayNetworkMontage(UAnimMontage* Mongtage, float PlayRate, int idx);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MultiPlayNetworkMontage(UAnimMontage* Mongtage, float PlayRate, int idx);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerPlayMontage(UAnimMontage* Mongtage, float PlayRate, int idx);

protected:
	/** Montage List */
	UPROPERTY(EditDefaultsOnly, Category = "AttackMontage")
	TArray<FAttackMontageStruct> MontageArr;
#pragma endregion
};
