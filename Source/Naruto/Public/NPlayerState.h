#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NPlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnPlayerHealthChangeDelegate);	//체력 변경

UCLASS()
class NARUTO_API ANPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ANPlayerState();
	float GetHealth() const;
	void InitPlayerData();

	UFUNCTION()
	void SetHealth(float val);

	FOnPlayerHealthChangeDelegate OnPlayerHealthChanged;		//데이터가 변동되면, UI 또한 연동되로고 델리게이트 설정

protected:
	UPROPERTY(VisibleAnywhere,Transient)
	float Health;
};
