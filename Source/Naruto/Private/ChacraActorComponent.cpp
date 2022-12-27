#include "ChacraActorComponent.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"

UChacraActorComponent::UChacraActorComponent(){
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true);

	ChacraCnt = 0;
}
void UChacraActorComponent::BeginPlay(){
	Super::BeginPlay();
		
}
void UChacraActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction){
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}
void UChacraActorComponent::UseChacra() {
	if (ChacraCnt < 2) {
		ChacraCnt++;
		UE_LOG(LogTemp, Warning, TEXT("Chacra : %d"), ChacraCnt);
		GetWorld()->GetTimerManager().ClearTimer(ResetChacraHandle);
		GetWorld()->GetTimerManager().SetTimer(ResetChacraHandle, this, &UChacraActorComponent::ResetChacraCnt, 3.f,false);
	}
}
void UChacraActorComponent::ResetChacraCnt() {
	ChacraCnt = 0;
	GetWorld()->GetTimerManager().ClearTimer(ResetChacraHandle);
	UE_LOG(LogTemp, Warning, TEXT("Reset Chacra"));
}
void UChacraActorComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UChacraActorComponent, ChacraCnt);
}