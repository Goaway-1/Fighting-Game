#include "MontageManager.h"
#include "Net/UnrealNetwork.h"

UMontageManager::UMontageManager(){
	PrimaryComponentTick.bCanEverTick = false;
}
void UMontageManager::BeginPlay(){
	Super::BeginPlay();
}
FName UMontageManager::GetAttackMontageSection(int32 Section) {
	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}
void UMontageManager::PlayNetworkMontage(UAnimMontage* Mongtage, float PlayRate, EPlayerCondition Condition, int idx) {
	if (MainAnimInstance) {
		MainAnimInstance->Montage_Play(Mongtage, PlayRate);
		if (Condition == EPlayerCondition::EPC_Attack || Condition == EPlayerCondition::EPC_Hited) {
			MainAnimInstance->Montage_JumpToSection(GetAttackMontageSection(idx), Mongtage);
		}
		else if (Condition == EPlayerCondition::EPC_Grap && idx == 1) {	//弊乏 付公府
			MainAnimInstance->Montage_JumpToSection("SuccessedAttack", Mongtage);
		}
	}

	ServerPlayMontage(Mongtage, PlayRate, Condition, idx);
}
void UMontageManager::MultiPlayNetworkMontage_Implementation(UAnimMontage* Mongtage,float PlayRate, EPlayerCondition Condition, int idx) {
	if (MainAnimInstance) {
		MainAnimInstance->Montage_Play(Mongtage, PlayRate);
		if (Condition == EPlayerCondition::EPC_Attack || Condition == EPlayerCondition::EPC_Hited) {
			MainAnimInstance->Montage_JumpToSection(GetAttackMontageSection(idx), Mongtage);
		}
		else if(Condition == EPlayerCondition::EPC_Grap && idx == 1) {	//弊乏 付公府
			MainAnimInstance->Montage_JumpToSection("SuccessedAttack", Mongtage);
		}
	}
}
bool UMontageManager::MultiPlayNetworkMontage_Validate(UAnimMontage* Mongtage, float PlayRate, EPlayerCondition Condition, int idx) {
	return true;
}
void UMontageManager::ServerPlayMontage_Implementation(UAnimMontage* Mongtage, float PlayRate, EPlayerCondition Condition,  int idx) {
	MultiPlayNetworkMontage(Mongtage, PlayRate, Condition,idx);
}
bool UMontageManager::ServerPlayMontage_Validate(UAnimMontage* Mongtage, float PlayRate, EPlayerCondition Condition, int idx) {
	return true;
}
