#include "PlayerStateWidget.h"
#include "NPlayerState.h"
#include "NPlayerController.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UPlayerStateWidget::UpdateState(EWidgetState state ,float value) {
	FNumberFormattingOptions Opts;
	Opts.SetMaximumFractionalDigits(0);

	if(state == EWidgetState::EWS_Switch)StateBar->SetPercent(value * 0.25f);
	else StateBar->SetPercent(value / 100.f);

	CurrentStateLabel->SetText(FText::AsNumber(value, &Opts));
};