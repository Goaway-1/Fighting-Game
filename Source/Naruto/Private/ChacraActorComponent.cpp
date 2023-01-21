#include "ChacraActorComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "NPlayer.h"
#include "NPlayerController.h"
#include "Particles/ParticleSystemComponent.h"
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
		// Setup Particle
		ServerDestroyCurParticle();
		if (ChacraCnt++ == 0) SpawnCurParticle(ChacraParticles.ChacraActive1);
		else SpawnCurParticle(ChacraParticles.ChacraActive2);

		// Reset Chacra Timer
		FTimerDelegate ResetChacra;
		ResetChacra.BindUFunction(this, FName("ResetChacraCnt"), false);
		GetWorld()->GetTimerManager().ClearTimer(ResetChacraHandle);
		GetWorld()->GetTimerManager().SetTimer(ResetChacraHandle, ResetChacra, 2.f, false);
	}
}
void UChacraActorComponent::ResetChacraCnt_Implementation(bool bIsUsed) {
	UE_LOG(LogTemp, Warning, TEXT("[Chacra]Reset Cnt"));

	/** Chacra consumption if used the skill */
	if (bIsUsed && ChacraCnt > 0) {
		CurrentChacra -= (ChacraCnt * 30.f);

		// Set Widget & Particle
		if(OwnPlayer.IsValid())	OwnPlayer->UpdateWidget(EWidgetState::EWS_Chacra);
		ServerDestroyCurParticle();
	}
	ChacraCnt = 0;
	GetWorld()->GetTimerManager().ClearTimer(ResetChacraHandle);
}
void UChacraActorComponent::ChargingChacra_Implementation() {
	if (CurrentChacra < MaxChacra) {
		CurrentChacra += ChargingVal;

		// Set Widget & Particle
		if (OwnPlayer.IsValid()) OwnPlayer->UpdateWidget(EWidgetState::EWS_Chacra);
		SpawnCurParticle(ChacraParticles.ChacraCharging);
	}
}
void UChacraActorComponent::SpawnCurParticle_Implementation(UParticleSystem* NewParticle) {
	if (CurParticle == nullptr) {
		if(NewParticle != ChacraParticles.ChacraCharging) CurParticle = UGameplayStatics::SpawnEmitterAttached(NewParticle, OwnPlayer->GetMesh(),FName(TEXT("Chacra")),FVector::Zero(),FRotator(-90.f,0.f,0.f), EAttachLocation::SnapToTarget);
		else CurParticle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), NewParticle, GetOwner()->GetActorLocation(), FRotator(0.f), true);
	}
}
bool UChacraActorComponent::SpawnCurParticle_Validate(UParticleSystem* NewParticle) {
	return true;
}
void UChacraActorComponent::ServerDestroyCurParticle_Implementation() {
	MultiDestroyCurParticle();
}
bool UChacraActorComponent::ServerDestroyCurParticle_Validate() {
	return true;
}
void UChacraActorComponent::MultiDestroyCurParticle_Implementation() {
	if (CurParticle) {
		CurParticle->DestroyComponent();
		CurParticle = nullptr;
	}
}
bool UChacraActorComponent::MultiDestroyCurParticle_Validate() {
	return true;
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