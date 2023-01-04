#include "NPlayerController.h"
#include "CutSceneWidget.h"
#include "NPlayer.h"
#include "MontageManager.h"
#include "Blueprint/UserWidget.h"

ANPlayerController::ANPlayerController() {
	
}
void ANPlayerController::BeginPlay() {
	if (IsLocalController() && MainSource && CutSceneSource) {
		PlayerWidget = CreateWidget<UUserWidget>(this, MainSource);
		CutSceneWidget = CreateWidget<UCutSceneWidget>(this, CutSceneSource);
		if (PlayerWidget && CutSceneWidget) {
			PlayerWidget->AddToViewport();
			PlayerWidget->SetVisibility(ESlateVisibility::Visible);
			CutSceneWidget->AddToViewport();
			CutSceneWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void ANPlayerController::PlayCutScene(UMediaSource* Source, UAnimMontage* Mongtage) {
	UE_LOG(LogTemp, Warning, TEXT("Start Skill Cut Scene"));
	CutSceneWidget->SetVisibility(ESlateVisibility::Visible);
	CutSceneWidget->PlayCutScene(Source);

	/** Set Montage & Condition */
	EndMongtage = Mongtage;			
	ANPlayer* OwnPlayer = Cast<ANPlayer>(GetCharacter());
	OwnPlayer->SetPlayerCondition(EPlayerCondition::EPC_CantMove);
}
void ANPlayerController::EndCutScene() {
	if (EndMongtage) {
		ANPlayer* OwnPlayer = Cast<ANPlayer>(GetCharacter());
		OwnPlayer->GetMontageManager()->PlayNetworkMontage(EndMongtage, 1.f, EPlayerCondition::EPC_Idle,0);
	}
}