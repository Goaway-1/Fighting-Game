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
};
