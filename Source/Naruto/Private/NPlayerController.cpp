#include "NPlayerController.h"
#include "CutSceneWidget.h"
#include "NPlayer.h"
#include "NPlayerState.h"
#include "HealthManager.h"
#include "MontageManager.h"
#include "PlayerStateWidget.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"

ANPlayerController::ANPlayerController() {

}
void ANPlayerController::BeginPlay() {
	//if (!HasAuthority() && MainWidgetClass && CutSceneWidgetClass) {
	if (IsLocalPlayerController() && MainWidgetClass && CutSceneWidgetClass) {
		MainWidget = CreateWidget<UUserWidget>(this, MainWidgetClass);
		CutSceneWidget = CreateWidget<UCutSceneWidget>(this, CutSceneWidgetClass);
		if (MainWidget && CutSceneWidget) {
			MainWidget->AddToViewport();
			MainWidget->SetVisibility(ESlateVisibility::Visible);
			CutSceneWidget->AddToViewport();
			CutSceneWidget->SetVisibility(ESlateVisibility::Hidden);

			// Set Health
			HealthWidget = MainWidget->WidgetTree->FindWidget<UPlayerStateWidget>("BP_OwnerHealth");


			// 응애
			//SetHealthWidget();
			//SetMultiHealthWidget();

			// 체력 초기 설정과 UI갱신
		}
	}
	//ANPlayerState* ABPlayerState = Cast<ANPlayerState>(PlayerState);
	////UE_LOG(LogTemp, Warning, TEXT("@#@@ %s"), *ABPlayerState->GetName());
	//if (ABPlayerState) {
	//	//UE_LOG(LogTemp, Warning,TEXT("@#@@ %s"),*GetCharacter()->GetName());
	//	HealthWidget->BindPlayerState(ABPlayerState);
	//	ABPlayerState->OnPlayerHealthChanged.Broadcast();
	//}
	SetSeverHealthWidget();
}
void ANPlayerController::SetHealthWidget_Implementation() {
	ANPlayerState* ABPlayerState = Cast<ANPlayerState>(PlayerState);
	if (ABPlayerState) {
		UE_LOG(LogTemp, Warning, TEXT("@#@@ %s"), *GetCharacter()->GetName());
		HealthWidget->BindPlayerState(ABPlayerState);
		ABPlayerState->OnPlayerHealthChanged.Broadcast();
	}
	//SetSeverHealthWidget();
}
void ANPlayerController::SetMultiHealthWidget_Implementation(){
	ANPlayerState* ABPlayerState = Cast<ANPlayerState>(PlayerState);
	if (ABPlayerState) {
		HealthWidget->BindPlayerState(ABPlayerState);
		ABPlayerState->OnPlayerHealthChanged.Broadcast();
	}
}
bool ANPlayerController::SetMultiHealthWidget_Validate() {
	return true;
}
void ANPlayerController::SetSeverHealthWidget_Implementation() {
	//SetMultiHealthWidget();
	ANPlayerState* ABPlayerState = Cast<ANPlayerState>(PlayerState);
	if (ABPlayerState) {
		UE_LOG(LogTemp, Warning, TEXT("@#@@ %s"), *ABPlayerState->GetName());
		HealthWidget->BindPlayerState(ABPlayerState);
		//ABPlayerState->OnPlayerHealthChanged.Broadcast();
	}
}
bool ANPlayerController::SetSeverHealthWidget_Validate() {
	return true;
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