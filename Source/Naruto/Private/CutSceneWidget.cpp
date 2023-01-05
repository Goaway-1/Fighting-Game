#include "CutSceneWidget.h"
#include "MediaPlayer.h"
#include "NPlayerController.h"
#include "Components/Image.h"

void UCutSceneWidget::NativeConstruct() {
}

void UCutSceneWidget::PlayCutScene(UMediaSource* Source, float MediaLength) {
	if(Source) {
		UE_LOG(LogTemp, Warning, TEXT("Sceneing.."));
		MediaPlayer->OpenSource(Source);
		VideoImage->SetVisibility(ESlateVisibility::Visible);	

		/** Set End Scene Timer... */
		GetWorld()->GetTimerManager().ClearTimer(StopMediaHandle);
		GetWorld()->GetTimerManager().SetTimer(StopMediaHandle, this, &UCutSceneWidget::StopCutScene, MediaLength, false);
	}
	else UE_LOG(LogTemp, Error, TEXT("[PlayCutScene] There is no MediaSource"))
}

void UCutSceneWidget::StopCutScene() {
	UE_LOG(LogTemp, Warning, TEXT("End Scene"));
	MediaPlayer->Rewind();
	MediaPlayer->Pause();
	VideoImage->SetVisibility(ESlateVisibility::Hidden);

	// Play Animation Monatge if End Cut Scene..
	ANPlayerController* OnwerController =  Cast<ANPlayerController>(GetOwningPlayer());
	if(OnwerController) OnwerController->EndCutScene();
}