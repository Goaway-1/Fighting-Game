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
void UMontageManager::PlayNetworkMontage(UAnimMontage* Mongtage, float PlayRate, bool isSkill, int idx) {
	if (MainAnimInstance) {
		MainAnimInstance->Montage_Play(Mongtage, PlayRate);
		if (!isSkill) {
			MainAnimInstance->Montage_JumpToSection(GetAttackMontageSection(idx), Mongtage);
		}
		else if (idx == 1) {	//스킬 마무리
			MainAnimInstance->Montage_JumpToSection("SuccessedAttack", Mongtage);
		}
	}

	ServerPlayMontage(Mongtage, PlayRate, isSkill, idx);
}
void UMontageManager::MultiPlayNetworkMontage_Implementation(UAnimMontage* Mongtage,float PlayRate, bool isSkill, int idx) {
	if (MainAnimInstance) {
		MainAnimInstance->Montage_Play(Mongtage, PlayRate);
		if (!isSkill) {
			MainAnimInstance->Montage_JumpToSection(GetAttackMontageSection(idx), Mongtage);
		}
		else if(idx == 1) {	//스킬 마무리
			MainAnimInstance->Montage_JumpToSection("SuccessedAttack", Mongtage);
		}
	}
}
bool UMontageManager::MultiPlayNetworkMontage_Validate(UAnimMontage* Mongtage, float PlayRate, bool isSkill, int idx) {
	return true;
}
void UMontageManager::ServerPlayMontage_Implementation(UAnimMontage* Mongtage, float PlayRate, bool isSkill, int idx) {
	MultiPlayNetworkMontage(Mongtage, PlayRate, isSkill,idx);
}
bool UMontageManager::ServerPlayMontage_Validate(UAnimMontage* Mongtage, float PlayRate, bool isSkill, int idx) {
	return true;
}
