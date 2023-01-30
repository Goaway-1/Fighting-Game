#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerStateWidget.generated.h"

/**
 * Health, Chacra, SideStep, WonCount
 */
UCLASS(Abstract)
class NARUTO_API UPlayerStateWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* StateBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CurrentStateLabel;

public:
	UFUNCTION()
	void UpdateState(EWidgetState state, float value);

private:
	TWeakObjectPtr<class ANPlayerState> CurrentPlayerState;
};
