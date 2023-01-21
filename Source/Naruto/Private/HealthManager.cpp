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
void UHealthManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction){
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}
void UHealthManager::SetDecreaseHealth(int8 val) {
	CurrentHealth -= val;
	SetHealthRatio();
	//UE_LOG(LogTemp, Warning, TEXT("Hited %f damage -> %f"), val, CurrentHealth);
}
bool UHealthManager::GetIsDead() {
	return (CurrentHealth <= 0) ? true : false;
}
void UHealthManager::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthManager, CurrentHealth);
}