#include "MontageManager.h"
#include "AttackStruct.h"
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
		if (Condition == EPlayerCondition::EPC_Attack || Condition == EPlayerCondition::EPC_UpperAttack || Condition == EPlayerCondition::EPC_AirAttack || Condition == EPlayerCondition::EPC_Hited || Condition == EPlayerCondition::EPC_UpperHited) {
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
		if (Condition == EPlayerCondition::EPC_Attack || Condition == EPlayerCondition::EPC_UpperAttack || Condition == EPlayerCondition::EPC_AirAttack || Condition == EPlayerCondition::EPC_Hited || Condition == EPlayerCondition::EPC_UpperHited) {
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
void UMontageManager::StopNetworkMontage() {
	if (MainAnimInstance) MainAnimInstance->Montage_Stop(0.f);

	ServerStopMontage();
}
void UMontageManager::MultiStopNetworkMontage_Implementation() {
	if (MainAnimInstance) MainAnimInstance->Montage_Stop(0.f);
}
bool UMontageManager::MultiStopNetworkMontage_Validate() {
	return true;
}
void UMontageManager::ServerStopMontage_Implementation() {
	MultiStopNetworkMontage();
}
bool UMontageManager::ServerStopMontage_Validate(){
	return true;
}
//void UMontageManager::SetActionMontage(int idx) {
//	UE_LOG(LogTemp,Warning,TEXT("SetActionMontage %d"), idx);
//	if(ActionMontages.Num() == 2) ActionMontage = ActionMontages[idx];
//	//ClSetActionMontage(idx);
//}
//void UMontageManager::ClSetActionMontage_Implementation(int idx) {
//	if (ActionMontages.Num() == 2) ActionMontage = ActionMontages[idx];
//}
//bool UMontageManager::ClSetActionMontage_Validate(int idx) {
//	return true;
//}