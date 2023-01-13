#include "NGameMode.h"
#include "NPlayer.h"
#include "NPlayerState.h"
#include "NPlayerController.h"
#include "NCameraManager.h"

ANGameMode::ANGameMode() {
	PlayerStateClass = ANPlayerState::StaticClass();
}
void ANGameMode::StartPlay() {
	Super::StartPlay();

}
void ANGameMode::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
}
void ANGameMode::PostLogin(APlayerController* NewPlayer) {
	Super::PostLogin(NewPlayer);

	ANPlayerState* MainlayerState = Cast<ANPlayerState>(NewPlayer->PlayerState);
	MainlayerState->InitPlayerData();
}