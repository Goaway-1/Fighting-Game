#include "NGameMode.h"
#include "NPlayer.h"
#include "NPlayerState.h"
#include "NPlayerController.h"
#include "NCameraManager.h"

ANGameMode::ANGameMode() {
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
	PlayerStateClass = ANPlayerState::StaticClass();

	RoundState = "Ready";
	PlayerCnt = 0;
	NumRounds = 2;
	RoundTime = 99;
	bIsTimerActive = false;
}
void ANGameMode::StartPlay() {
	Super::StartPlay();

}
void ANGameMode::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	/** Start Timer */
	if (bIsTimerActive) {
		if(RoundTime - DeltaSeconds >= 0) RoundTime -= DeltaSeconds;
		else RoundTime = 0;
	}
}
void ANGameMode::PostLogin(APlayerController* NewPlayer) {
	Super::PostLogin(NewPlayer);

	ANPlayerState* MainlayerState = Cast<ANPlayerState>(NewPlayer->PlayerState);
	MainlayerState->InitPlayerData();
	PlayerCnt++;

	/** Round start when all players enter */
	if (PlayerCnt >= 2) {
		GetWorld()->GetTimerManager().ClearTimer(RoundStartHandle);
		GetWorld()->GetTimerManager().SetTimer(RoundStartHandle, this, &ANGameMode::RoundStart,2.f);
	}
}
void ANGameMode::RoundStart() {
	bIsTimerActive = true;
	RoundState = "Fight";

	GetWorld()->GetTimerManager().ClearTimer(RoundStartHandle);
	GetWorld()->GetTimerManager().SetTimer(RoundStartHandle, this, &ANGameMode::RoundStartEnd, 0.5f);
}
void ANGameMode::RoundStartEnd() {
	RoundState = " ";
}
void ANGameMode::RoundEnd() {
	ResetValue();

	GetWorld()->GetTimerManager().ClearTimer(RoundStartHandle);
	GetWorld()->GetTimerManager().SetTimer(RoundStartHandle, this, &ANGameMode::RoundStart, 3.f);
}
void ANGameMode::ResetValue() {
	RoundState = "Ready";
	RoundTime = 99;
	bIsTimerActive = false;
}
void ANGameMode::GameOver() {	
	// Who is Winner?
	int idx = 1;
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++){
		auto PlayerSt = Cast<ANPlayerState>(It->Get()->GetCharacter()->GetPlayerState());
		
		if (PlayerSt->GetState(EWidgetState::EWS_Score) == 2.f) {
			RoundState = "Player ";
			RoundState += FString::FromInt(idx);
			RoundState += " is Win!!";
			bIsTimerActive = false;
			return;
		}
		else idx++;
	}
}