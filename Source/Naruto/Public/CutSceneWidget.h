#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CutSceneWidget.generated.h"

/**
 * Play CutScene when Chacra Skill Hited!
 */
UCLASS(Abstract)
class NARUTO_API UCutSceneWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	class UImage* VideoImage;

	UPROPERTY(EditAnywhere, category="CutScene")
	class UMediaPlayer* MediaPlayer;

	UPROPERTY()
	FTimerHandle StopMediaHandle;
public:
	UFUNCTION()
	void PlayCutScene(UMediaSource* Source, float MediaLength);

	UFUNCTION()
	void StopCutScene();
};