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
	UPROPERTY()
	UAnimInstance* MainAnimInstance;

	UPROPERTY()
	class ANPlayer* CurOwner;

public:
	FORCEINLINE void SetInit(ANPlayer* player, UAnimInstance* AnimInst) { CurOwner = player; MainAnimInstance = AnimInst; }

	UMontageManager();

	FName GetAttackMontageSection(int32 Section);	// Retrun MontageNumber

	void PlayNetworkMontage(UAnimMontage* Mongtage, float PlayRate, EPlayerCondition Condition, int idx = 0);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MultiPlayNetworkMontage(UAnimMontage* Mongtage, float PlayRate, EPlayerCondition Condition, int idx = 0);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerPlayMontage(UAnimMontage* Mongtage, float PlayRate, EPlayerCondition Condition, int idx = 0);

	/** For Access from NWeapon */
	UFUNCTION()
	FORCEINLINE FAttackMontageStruct GetActionMontage() { return ActionMontage; }

	UFUNCTION()
	FORCEINLINE bool IsMontagePlaying(UAnimMontage* Montage) { return (MainAnimInstance->Montage_IsPlaying(Montage)) ? true:false ; }

	UFUNCTION()
	FORCEINLINE void StopMontage() { MainAnimInstance->StopAllMontages(0.f); }

protected:
	/** Montage List */
	UPROPERTY(EditDefaultsOnly, Category = "AttackMontage")
	FAttackMontageStruct ActionMontage;
		
};
