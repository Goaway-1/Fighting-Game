#include "HealthManager.h"
#include "Net/UnrealNetwork.h"

UHealthManager::UHealthManager(){
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true);

    CurrentHealth = MaxHealth;
}
void UHealthManager::BeginPlay(){
	Super::BeginPlay();
	
}
void UHealthManager::SetDecreaseHealth(int8 val) {
	CurrentHealth -= val;
	if(CurrentHealth <= 0.f) CurrentHealth = 0.f;
}
bool UHealthManager::GetIsDead() {
	return (CurrentHealth <= 0) ? true : false;
}
void UHealthManager::RecoverHealth() {
	CurrentHealth = MaxHealth;
}
void UHealthManager::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UHealthManager, CurrentHealth, COND_OwnerOnly);
}