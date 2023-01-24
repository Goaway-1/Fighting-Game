#include "NGameMode.h"
#include "NPlayer.h"
#include "NPlayerState.h"
#include "NPlayerController.h"
#include "NCameraManager.h"

ANGameMode::ANGameMode() {
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
	PlayerStateClass = ANPlayerState::StaticClass();

	CurrentPlayerCnt = 0;
	NumRounds = 2;
	RoundTime = 99;
	bIsTimerActive = false;
}
void ANGameMode::StartPlay() {
	Super::StartPlay();

}
void ANGameMode::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	// 시간의 흐름..
	if (bIsTimerActive) {
		if(RoundTime - DeltaSeconds >= 0) RoundTime -= DeltaSeconds;
		else RoundTime = 0;
	}
}
void ANGameMode::PostLogin(APlayerController* NewPlayer) {
	Super::PostLogin(NewPlayer);

	ANPlayerState* MainlayerState = Cast<ANPlayerState>(NewPlayer->PlayerState);
	MainlayerState->InitPlayerData();
	CurrentPlayerCnt++;

	// 모든 플레이어 진입시..
	// 플레이어 컨트롤러에서 함수 실행 & 여기서 라운드 스타트 넘김..
	if (CurrentPlayerCnt >= 2) {
		UE_LOG(LogTemp,Warning,TEXT("All Player PostLogin!!"));
		RoundStart();
	}
}
void ANGameMode::RoundStart() {
	bIsTimerActive = true;
}