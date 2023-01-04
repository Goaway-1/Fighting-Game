#include "CutSceneWidget.h"
#include "MediaPlayer.h"
#include "NPlayerController.h"
#include "Components/Image.h"

void UCutSceneWidget::NativeConstruct() {
	MediaPlayer->OnEndReached.AddDynamic(this, &UCutSceneWidget::StopCutScene);
}

void UCutSceneWidget::PlayCutScene(UMediaSource* Source) {
	if(Source) {
		UE_LOG(LogTemp, Warning, TEXT("Sceneing.."));
		MediaPlayer->OpenSource(Source);
		VideoImage->SetVisibility(ESlateVisibility::Visible);	
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