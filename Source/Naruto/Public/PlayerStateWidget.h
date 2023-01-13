#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerStateWidget.generated.h"

/**
 * Health & Chacra
 */
UCLASS(Abstract)
class NARUTO_API UPlayerStateWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(Client, Reliable)
	//UFUNCTION()
	void BindPlayerState(class ANPlayerState* PlayerState);

protected:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CurrentHealthLabel;

	UFUNCTION()
	void UpdatePlayerState();

private:
	//TWeakObjectPtr<class ANPlayerState> CurrentPlayerState;
	class ANPlayerState* CurrentPlayerState;
};
