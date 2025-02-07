#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttackStruct.h"
#include "NPlayer.h"
#include "MontageManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NARUTO_API UMontageManager : public UActorComponent
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;

protected:
	UAnimInstance* MainAnimInstance;

public:
	FORCEINLINE void SetInit(UAnimInstance* AnimInst) { MainAnimInstance = AnimInst; }

	UMontageManager();

	FName GetAttackMontageSection(int32 Section);	// Retrun MontageNumber

	/** For Access from NWeapon */
	UFUNCTION()
	FORCEINLINE FAttackMontageStruct GetActionMontage() { return ActionMontage; }

	UFUNCTION()
	FORCEINLINE bool IsMontagePlaying(UAnimMontage* Montage) { return (MainAnimInstance->Montage_IsPlaying(Montage)) ? true:false ; }

	/**
	* Functions that process animation
	* @param Mongtage   Animation want to run..
	* @param PlayRate   Animation PlayRate..
	* @param Condition  Current Player's Condition (EPlayerCondtiion)
	* @param idx        Attack Count Index..  
	*/
	UFUNCTION()
	void PlayNetworkMontage(UAnimMontage* Mongtage, float PlayRate, EPlayerCondition Condition, int idx = 0);

	UFUNCTION()
	void StopNetworkMontage();
private:
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MultiPlayNetworkMontage(UAnimMontage* Mongtage, float PlayRate, EPlayerCondition Condition, int idx = 0);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerPlayMontage(UAnimMontage* Mongtage, float PlayRate, EPlayerCondition Condition, int idx = 0);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MultiStopNetworkMontage();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStopMontage();

	/** Set Montage by Weapon type.. */
	UFUNCTION(BlueprintCallable)
	void SetActionMontage(bool btype);

	FAttackMontageStruct ActionMontage;
protected:
	/** Montage List */
	UPROPERTY(EditDefaultsOnly, Category = "AttackMontage")
	TArray<FAttackMontageStruct> ActionMontages;
};
