#include "AttackActorComponent.h"
#include "NPlayer.h"
#include "Net/UnrealNetwork.h"

UAttackActorComponent::UAttackActorComponent(){
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true);

	ComboCnt = 1;
	bAttacking = false;
	bIsAttackCheck = false;
	CurKeyUD = EKeyUpDown::EKUD_Default;
	TmpKeyUD = EKeyUpDown::EKUD_Default;
}
void UAttackActorComponent::BeginPlay(){
	Super::BeginPlay();

}
void UAttackActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction){
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}
void UAttackActorComponent::DefaultAttack_KeyDown(EKeyUpDown KeyUD){
	TmpKeyUD = KeyUD;

	if (!bAttacking) Attack();
	else bIsAttackCheck = true;
}
void UAttackActorComponent::SetOverlapActors(AActor* actor) {
	OverlapActors.Add(actor);
}
TArray<AActor*> UAttackActorComponent::GetOverlapActors() {
	return OverlapActors;
}
bool UAttackActorComponent::IsAlreadyOverlap(AActor* actor) { 
	for (int i = 0; i< OverlapActors.Num();i++) { 
		if (OverlapActors[i] == actor) return true;
	} 
	return false;
}
void UAttackActorComponent::Attack() {
	bAttacking = true;

	if (MainAnimInstance) {
		if (!MainAnimInstance->Montage_IsPlaying(MontageArr[0].MT_Attacker) && !MainAnimInstance->Montage_IsPlaying(MontageArr[0].AttackSplit.MTUP_Attacker)) {	//공격중이 아닐때 (처음 공격)
			ComboCnt = 1;
			PlayNetworkMontage(MontageArr[0].MT_Attacker, 1.f, 1);
		}
		else {
			if (CurKeyUD == EKeyUpDown::EKUD_Up) PlayNetworkMontage(MontageArr[0].AttackSplit.MTUP_Attacker, 1.f, ComboCnt);
			else if (CurKeyUD == EKeyUpDown::EKUD_Down) PlayNetworkMontage(MontageArr[0].AttackSplit.MTDOWN_Attacker, 1.f, ComboCnt);
			else PlayNetworkMontage(MontageArr[0].MT_Attacker, 1.f, ComboCnt);
		}
	}
}
void UAttackActorComponent::EndAttack() {
	bAttacking = false;
	CurKeyUD = EKeyUpDown::EKUD_Default;
	ComboCnt = 1;
}
void UAttackActorComponent::AttackInputCheck() {
	if (bIsAttackCheck) {
		ComboCnt++;
		if (MontageArr[0].splitIdx == ComboCnt) CurKeyUD = TmpKeyUD;
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
void UAttackActorComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAttackActorComponent, OverlapActors);
}