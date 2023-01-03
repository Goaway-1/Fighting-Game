#include "NPlayerController.h"
#include "Blueprint/UserWidget.h"

ANPlayerController::ANPlayerController() {
	
}
void ANPlayerController::BeginPlay() {
	if (NewIntroWidget) {
		PlayerWidget = CreateWidget<UUserWidget>(this, NewIntroWidget);
		if (PlayerWidget) {
			PlayerWidget->AddToViewport();
			PlayerWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
}