#include "ChacraActorComponent.h"
#include "GameFramework/Actor.h"
#include "NPlayer.h"
#include "NPlayerController.h"
#include "Net/UnrealNetwork.h"

UChacraActorComponent::UChacraActorComponent(){
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true);

	ChacraCnt = 0;
}
void UChacraActorComponent::BeginPlay(){
	Super::BeginPlay();

	OwnPlayer = Cast<ANPlayer>(GetOwner());
}
void UChacraActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction){
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}
void UChacraActorComponent::UseChacra_Implementation() {
	if ((ChacraCnt == 0 && CurrentChacra >= 30.f) || (ChacraCnt == 1 && CurrentChacra >= 60.f)) {
		ChacraCnt++;

		//UE_LOG(LogTemp, Warning, TEXT("Chacra : %d"), ChacraCnt);

		FTimerDelegate ResetChacra;
		ResetChacra.BindUFunction(this, FName("ResetChacraCnt"), false);
		GetWorld()->GetTimerManager().ClearTimer(ResetChacraHandle);
		GetWorld()->GetTimerManager().SetTimer(ResetChacraHandle, ResetChacra, 3.f, false);
	}
}
void UChacraActorComponent::ResetChacraCnt_Implementation(bool bIsUsed) {
	UE_LOG(LogTemp, Warning, TEXT("[Chacra]Reset Cnt"));

	/** Chacra consumption if used the skill */
	if (bIsUsed && ChacraCnt > 0) {
		CurrentChacra -= (ChacraCnt * 30.f);
		//UE_LOG(LogTemp, Warning, TEXT("[Chacra]Used Chacra"));

		// Set Widget
		if(OwnPlayer.IsValid())	OwnPlayer->UpdateWidget(EWidgetState::EWS_Chacra);
	}
	ChacraCnt = 0;
	GetWorld()->GetTimerManager().ClearTimer(ResetChacraHandle);
}
void UChacraActorComponent::ChargingChacra_Implementation() {
	if (CurrentChacra < MaxChacra) {
		//UE_LOG(LogTemp, Warning, TEXT("[Chacra]Charging"));
		CurrentChacra += ChargingVal;

		// Set Widget
		if (OwnPlayer.IsValid()) OwnPlayer->UpdateWidget(EWidgetState::EWS_Chacra);
	}
}
bool UChacraActorComponent::UseChacra_Validate() {
	return true;
}
bool UChacraActorComponent::ResetChacraCnt_Validate(bool bIsUsed) {
	return true;
}
bool UChacraActorComponent::ChargingChacra_Validate() {
	return true;
}

void UChacraActorComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UChacraActorComponent, ChacraCnt);
	DOREPLIFETIME(UChacraActorComponent, CurrentChacra);
}