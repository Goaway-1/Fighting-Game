#pragma once

#include "CoreMinimal.h"
#include "NPlayer.h"
#include "GameFramework/PlayerState.h"
#include "NPlayerState.generated.h"

UCLASS()
class NARUTO_API ANPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ANPlayerState();
	float GetState(EWidgetState state)  const;
	void InitPlayerData();

	UFUNCTION()
	void SetState(EWidgetState state, float val);			// Set Values..
protected:
	float Health;
	float Chacra;
	int SideStep;
};
