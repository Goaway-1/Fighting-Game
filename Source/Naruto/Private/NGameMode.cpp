#include "NGameMode.h"
#include "NPlayer.h"
#include "NPlayerController.h"
#include "NCameraManager.h"

ANGameMode::ANGameMode() {

}
void ANGameMode::StartPlay() {
	Super::StartPlay();

}
void ANGameMode::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
}
void ANGameMode::PostLogin(APlayerController* NewPlayer) {
	Super::PostLogin(NewPlayer);

}