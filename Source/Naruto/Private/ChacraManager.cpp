#include "ChacraManager.h"
#include "NPlayer.h"
#include "NPlayerController.h"
#include "GameFramework/Actor.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

UChacraManager::UChacraManager(){
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicated(true);
}
void UChacraManager::BeginPlay(){
	Super::BeginPlay();

	OwnPlayer = Cast<ANPlayer>(GetOwner());
}
void UChacraManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction){
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}
void UChacraManager::UseChacra_Implementation() {
	if ((ChacraCnt == 0 && CurrentChacra >= 30.f) || (ChacraCnt == 1 && CurrentChacra >= 60.f)) {
		// Setup Particle
		ServerDestroyCurParticle();
		if (ChacraCnt++ == 0) SpawnCurParticle(ChacraParticles.ChacraActive1);
		else SpawnCurParticle(ChacraParticles.ChacraActive2);

		// SoundPlay...
		if(ChacraActive_Sound) UGameplayStatics::PlaySound2D(this,ChacraActive_Sound);

		// Reset Chacra Timer
		FTimerDelegate ResetChacra;
		ResetChacra.BindUFunction(this, FName("ResetChacraCnt"), EChacraActions::ECA_None, false);
		GetWorld()->GetTimerManager().ClearTimer(ResetChacraHandle);
		GetWorld()->GetTimerManager().SetTimer(ResetChacraHandle, ResetChacra, 2.f, false);
	}
}
bool UChacraManager::UseChacra_Validate() {
	return true;
}
void UChacraManager::ResetChacraCnt_Implementation(EChacraActions InputChacraActions, bool bIsUsed) {
	UE_LOG(LogTemp, Warning, TEXT("[Chacra]Reset Cnt"));

	/** Chacra consumption if used the skill */
	if (bIsUsed && ChacraCnt > 0) {
		if (InputChacraActions == EChacraActions::ECA_Dash || InputChacraActions == EChacraActions::ECA_NinjaStar) CurrentChacra -= NormalChacraConsumption;
		else CurrentChacra -= (ChacraCnt * SkillChacraConsumption);

		// Set Widget & Particle
		if(OwnPlayer.IsValid())	OwnPlayer->UpdateWidget(EWidgetState::EWS_Chacra);
		ServerDestroyCurParticle();
	}
	ChacraCnt = 0;
	GetWorld()->GetTimerManager().ClearTimer(ResetChacraHandle);
}
bool UChacraManager::ResetChacraCnt_Validate(EChacraActions InputChacraActions, bool bIsUsed) {
	return true;
}
void UChacraManager::ChargingChacra_Implementation() {
	if (CurrentChacra < MaxChacra) {
		CurrentChacra += ChargingVal;

		// Set Widget & Particle
		if (OwnPlayer.IsValid()) OwnPlayer->UpdateWidget(EWidgetState::EWS_Chacra);
		SpawnCurParticle(ChacraParticles.ChacraCharging);
	}
}
bool UChacraManager::ChargingChacra_Validate() {
	return true;
}
void UChacraManager::SpawnCurParticle_Implementation(UParticleSystem* NewParticle) {
	if (CurParticle == nullptr) {
		if(NewParticle != ChacraParticles.ChacraCharging) CurParticle = UGameplayStatics::SpawnEmitterAttached(NewParticle, OwnPlayer->GetMesh(),FName(TEXT("Chacra")),FVector::Zero(),FRotator(-90.f,0.f,0.f), EAttachLocation::SnapToTarget);
		else CurParticle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), NewParticle, GetOwner()->GetActorLocation(), FRotator(0.f), true);
	}
}
bool UChacraManager::SpawnCurParticle_Validate(UParticleSystem* NewParticle) {
	return true;
}
void UChacraManager::ServerDestroyCurParticle_Implementation() {
	MultiDestroyCurParticle();
}
bool UChacraManager::ServerDestroyCurParticle_Validate() {
	return true;
}
void UChacraManager::MultiDestroyCurParticle_Implementation() {
	if (CurParticle) {
		CurParticle->DestroyComponent();
		CurParticle = nullptr;
	}
}
bool UChacraManager::MultiDestroyCurParticle_Validate() {
	return true;
}
void UChacraManager::RecoverChacra() {
	ChacraCnt = 0;
	CurrentChacra = MaxChacra;
	ServerDestroyCurParticle();
}
void UChacraManager::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UChacraManager, ChacraCnt, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UChacraManager, CurrentChacra, COND_OwnerOnly);
}