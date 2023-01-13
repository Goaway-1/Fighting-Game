#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NPlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnPlayerHealthChangeDelegate);	//ü�� ����

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

	FOnPlayerHealthChangeDelegate OnPlayerHealthChanged;		//�����Ͱ� �����Ǹ�, UI ���� �����Ƿΰ� ��������Ʈ ����

protected:
	UPROPERTY(VisibleAnywhere,Transient)
	float Health;
};
