#pragma once

#include "EngineMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NGameMode.generated.h"

class ANCameraManager;
/**
 * 
 */
UCLASS()
class NARUTO_API ANGameMode : public AGameModeBase
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	ANCameraManager* MainCamera;
public:
	ANGameMode();

	virtual void StartPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

private:
	int PlayerCnt;					  // Current Player Count
	float RoundTime;				  // Current Timer
	bool bIsTimerActive;			  // Is Timer Active?
	int NumRounds;					  // Count of Win Round
	FString RoundState;				  // Crrent Round State : Ready, fight, none
	FTimerHandle RoundStartHandle; 	  // Round Start Handler

	class ANGameMode* GameModeReference;

	/** Round start when all players enter */
	UFUNCTION()
	void RoundStart();

	/** Reset Message */
	UFUNCTION()
	void RoundStartEnd();

	/** Reset Round Value */
	void ResetValue();       
public:
	FORCEINLINE bool GetIsTimerActive() { return bIsTimerActive; }
	FORCEINLINE FString GetRoundState() { return RoundState; }
	FORCEINLINE float GetRoundTime() {return RoundTime; }

	UFUNCTION()
	void RoundEnd();          // End Round And Restart.!!

	UFUNCTION()
	void GameOver();          // Game Over...

};
