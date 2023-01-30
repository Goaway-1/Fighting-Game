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
	void InitPlayerData();

	/** Set/GetState Value.. */
	UFUNCTION()
	void SetState(EWidgetState state, float val);			// Set Values..
	float GetState(EWidgetState state) const;				//
protected:
	/** Widget Data(State)'s Value */
	float Health;
	float Chacra;
	int SideStep;
	int Score;		           
};
