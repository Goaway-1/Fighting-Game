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

// @TODO: 테스트중 ... MODE>>..

public:
	int CurrentPlayerCnt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameMode Settings")
	float RoundTime;

	// 승리해야하는 라운드..
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameMode Settings")
	int NumRounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameMode Settings")
	bool bIsTimerActive;

	UFUNCTION()
	void RoundStart();
};
