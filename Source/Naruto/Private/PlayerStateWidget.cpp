#include "PlayerStateWidget.h"
#include "NPlayerState.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

//void UPlayerStateWidget::BindPlayerState(class ANPlayerState* PlayerState) {
void UPlayerStateWidget::BindPlayerState_Implementation(class ANPlayerState* PlayerState) {
	if (PlayerState) {
		CurrentPlayerState = PlayerState;		//¿¡·¯
		PlayerState->OnPlayerHealthChanged.AddUObject(this, &UPlayerStateWidget::UpdatePlayerState);
	}
}
void UPlayerStateWidget::UpdatePlayerState(){
	//if (CurrentPlayerState.IsValid()) {
	if (CurrentPlayerState) {
		//UE_LOG(LogTemp,Warning,TEXT("Update Player State : %f"), CurrentPlayerState->GetHealth());
		FNumberFormattingOptions Opts;
		Opts.SetMaximumFractionalDigits(0);

		HealthBar->SetPercent(CurrentPlayerState->GetHealth() / 100.f);
		CurrentHealthLabel->SetText(FText::AsNumber(CurrentPlayerState->GetHealth(), &Opts));
	}
};