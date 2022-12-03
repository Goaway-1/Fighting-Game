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

void UAttackActorComponent::Attack() {
	PlayNetworkMontage(AttackMontage, 1.f);
}

void UAttackActorComponent::PlayNetworkMontage(UAnimMontage* Mongtage, float PlayRate) {
	if (Cast<ACharacter>(GetOwner())->IsLocallyControlled()) {
		if (MainAnimInstance) MainAnimInstance->Montage_Play(Mongtage, PlayRate);

		ServerPlayMontage(Mongtage,PlayRate);
	}
}
void UAttackActorComponent::MultiPlayNetworkMontage_Implementation(UAnimMontage* Mongtage, float PlayRate) {
	if (!Cast<ACharacter>(GetOwner())->IsLocallyControlled()) {
		if (MainAnimInstance) MainAnimInstance->Montage_Play(Mongtage, PlayRate);
	}
}
bool UAttackActorComponent::MultiPlayNetworkMontage_Validate(UAnimMontage* Mongtage, float PlayRate) {
	return true;
}
void UAttackActorComponent::ServerPlayMontage_Implementation(UAnimMontage* Mongtage, float PlayRate) {
	MultiPlayNetworkMontage(Mongtage, PlayRate);
}
bool UAttackActorComponent::ServerPlayMontage_Validate(UAnimMontage* Mongtage, float PlayRate) {
	return true;
}