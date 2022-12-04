#include "AttackActorComponent.h"
#include "NPlayer.h"
#include "Net/UnrealNetwork.h"

UAttackActorComponent::UAttackActorComponent(){
	PrimaryComponentTick.bCanEverTick = false;

}
void UAttackActorComponent::BeginPlay(){
	Super::BeginPlay();

}
void UAttackActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction){
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}
void UAttackActorComponent::DefaultAttack_KeyDown() {
	bLMBDown = true;

	if (!bAttacking) Attack();
	else bIsAttackCheck = true;
}
void UAttackActorComponent::Attack() {
	bAttacking = true;

	if (MainAnimInstance) {
		if (!MainAnimInstance->Montage_IsPlaying(MontageArr[0].MT_Attacker)) {	//공격중이 아닐때 (처음 공격)
			ComboCnt = 1;
			PlayNetworkMontage(MontageArr[0].MT_Attacker, 1.f, 1);
		}
		else PlayNetworkMontage(MontageArr[0].MT_Attacker, 1.f, ComboCnt);
	}
}
void UAttackActorComponent::EndAttack() {
	bAttacking = false;
}
void UAttackActorComponent::AttackInputCheck() {
	if (bIsAttackCheck) {
		ComboCnt++;
		if (ComboCnt > MontageArr[0].ComboCnt) ComboCnt = 1;
		bIsAttackCheck = false;
		Attack();
	}
}
FName UAttackActorComponent::GetAttackMontageSection(int32 Section) {
	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}
void UAttackActorComponent::PlayNetworkMontage(UAnimMontage* Mongtage, float PlayRate, int idx) {
	if (Cast<ACharacter>(GetOwner())->IsLocallyControlled()) {
		if (MainAnimInstance) {
			MainAnimInstance->Montage_Play(Mongtage, PlayRate);
			MainAnimInstance->Montage_JumpToSection(GetAttackMontageSection(idx), Mongtage);
		}

		ServerPlayMontage(Mongtage,PlayRate,idx);
	}
}
void UAttackActorComponent::MultiPlayNetworkMontage_Implementation(UAnimMontage* Mongtage, float PlayRate, int idx) {
	if (!Cast<ACharacter>(GetOwner())->IsLocallyControlled()) {
		if (MainAnimInstance) {
			MainAnimInstance->Montage_Play(Mongtage, PlayRate);
			MainAnimInstance->Montage_JumpToSection(GetAttackMontageSection(idx), Mongtage);
		}
	}
}
bool UAttackActorComponent::MultiPlayNetworkMontage_Validate(UAnimMontage* Mongtage, float PlayRate, int idx) {
	return true;
}
void UAttackActorComponent::ServerPlayMontage_Implementation(UAnimMontage* Mongtage, float PlayRate, int idx) {
	MultiPlayNetworkMontage(Mongtage, PlayRate, idx);
}
bool UAttackActorComponent::ServerPlayMontage_Validate(UAnimMontage* Mongtage, float PlayRate, int idx) {
	return true;
}