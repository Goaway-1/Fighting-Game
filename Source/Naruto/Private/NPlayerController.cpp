#include "NPlayerController.h"
#include "CutSceneWidget.h"
#include "NPlayer.h"
#include "NGameStateBase.h"
#include "NPlayerState.h"
#include "NGameMode.h"
#include "HealthManager.h"
#include "MontageManager.h"
#include "PlayerStateWidget.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"

ANPlayerController::ANPlayerController() {
	HealthWidgets.SetNum(2);
	ChacraWidgets.SetNum(2);
	SideStepWidgets.SetNum(2);
	ScoreWidgets.SetNum(2);
	bIsRoundStart = false;
}
void ANPlayerController::BeginPlay() {
	if (IsLocalPlayerController() && MainWidgetClass && CutSceneWidgetClass) {
		MainWidget = CreateWidget<UUserWidget>(this, MainWidgetClass);
		CutSceneWidget = CreateWidget<UCutSceneWidget>(this, CutSceneWidgetClass);
		if (MainWidget && CutSceneWidget) {
			MainWidget->AddToViewport();
			MainWidget->SetVisibility(ESlateVisibility::Visible);
			CutSceneWidget->AddToViewport();
			CutSceneWidget->SetVisibility(ESlateVisibility::Hidden);

			// Set Health & Chacra 
			HealthWidgets[0] = MainWidget->WidgetTree->FindWidget<UPlayerStateWidget>("BP_P1Health");
			HealthWidgets[1] = MainWidget->WidgetTree->FindWidget<UPlayerStateWidget>("BP_P2Health");
			ChacraWidgets[0] = MainWidget->WidgetTree->FindWidget<UPlayerStateWidget>("BP_P1Chacra");
			ChacraWidgets[1] = MainWidget->WidgetTree->FindWidget<UPlayerStateWidget>("BP_P2Chacra");
			SideStepWidgets[0] = MainWidget->WidgetTree->FindWidget<UPlayerStateWidget>("BP_P1SideStep");
			SideStepWidgets[1] = MainWidget->WidgetTree->FindWidget<UPlayerStateWidget>("BP_P2SideStep");
			ScoreWidgets[0] = MainWidget->WidgetTree->FindWidget<UPlayerStateWidget>("BP_P1Score");
			ScoreWidgets[1] = MainWidget->WidgetTree->FindWidget<UPlayerStateWidget>("BP_P2Score");
	
			// Set Round info
			MiddleScreenText = MainWidget->WidgetTree->FindWidget<UTextBlock>("MiddleScreenText");
			RoundTimerText = MainWidget->WidgetTree->FindWidget<UTextBlock>("RoundTimerText");
			
			// Reset All Widget..
			ResetWidget();
		}
	}

	// Get GameMode..
	if (HasAuthority()) CurGameMode = Cast<ANGameMode>(GetWorld()->GetAuthGameMode());
}
void ANPlayerController::TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction) {
	Super::TickActor(DeltaTime,TickType,ThisTickFunction);

	if (CurGameMode) {
		if(CurGameMode->GetIsTimerActive()) SetRoundInfo(CurGameMode->GetRoundTime(), CurGameMode->GetRoundState());
		else if(CurGameMode->GetRoundState() != "Fight") SetRoundInfo(99, CurGameMode->GetRoundState(), true);
	}
}
void ANPlayerController::SetWidget_Implementation(const EWidgetState State) {
	int i = 0;
	for (auto s : GetWorld()->GetGameState()->PlayerArray){
		ANPlayerState* PState = Cast<ANPlayerState>(s);

		UpdateWidget(i++, State, PState->GetState(State));
	}
}
bool ANPlayerController::SetWidget_Validate(const EWidgetState State) {
	return true;
}
void ANPlayerController::UpdateWidget_Implementation(int idx, const EWidgetState State, float value){
	switch (State)
	{
	case EWidgetState::EWS_Health:
		HealthWidgets[idx]->UpdateState(State,value);
		break;
	case EWidgetState::EWS_Chacra:
		ChacraWidgets[idx]->UpdateState(State,value);
		break;
	case EWidgetState::EWS_Switch:
		SideStepWidgets[idx]->UpdateState(State,value);
		break;
	case EWidgetState::EWS_Score:
		ScoreWidgets[idx]->UpdateState(State, value);
		break;
	default:
		break;
	}
}
void ANPlayerController::ResetWidget_Implementation() {
	for (int i = 0; i < 2; i++){
		HealthWidgets[i]->UpdateState(EWidgetState::EWS_Health,100);
		ChacraWidgets[i]->UpdateState(EWidgetState::EWS_Chacra,100);
		SideStepWidgets[i]->UpdateState(EWidgetState::EWS_Switch,4);
		ScoreWidgets[i]->UpdateState(EWidgetState::EWS_Score,0);
	}
	MiddleScreenText->SetText(FText::FromString("READY?"));
	RoundTimerText->SetText(FText::FromString("99"));
}
void ANPlayerController::PlayCutScene(UMediaSource* Source, UAnimMontage* Mongtage, float MediaLength) {
	UE_LOG(LogTemp, Warning, TEXT("Start Skill Cut Scene"));
	CutSceneWidget->SetVisibility(ESlateVisibility::Visible);
	CutSceneWidget->PlayCutScene(Source, MediaLength);

	/** Set Montage & Condition */
	EndMongtage = Mongtage;			
}
void ANPlayerController::EndCutScene() {
	if (EndMongtage) {
		ANPlayer* OwnPlayer = Cast<ANPlayer>(GetCharacter());
		OwnPlayer->GetMontageManager()->PlayNetworkMontage(EndMongtage, 1.f, EPlayerCondition::EPC_Idle,0);
	}
}
void ANPlayerController::SetRoundInfo_Implementation(int time, const FString& text, bool bIsForce) {
	if (MiddleScreenText && RoundTimerText && bIsForce) {
		MiddleScreenText->SetText(FText::FromString(text));
		RoundTimerText->SetText(FText::FromString(FString::FromInt(time)));
		bIsRoundStart = false;
		return;
	}

	if (MiddleScreenText && RoundTimerText) {
		if(time >= 98) {
			MiddleScreenText->SetText(FText::FromString(text));
			bIsRoundStart = true;
		}
		RoundTimerText->SetText(FText::FromString(FString::FromInt(time)));
	}
}
void ANPlayerController::RequestRoundEnd() {
	if (CurGameMode) CurGameMode->RoundEnd();
}
void ANPlayerController::RequestGameOver() {
	if (CurGameMode) CurGameMode->GameOver();
}