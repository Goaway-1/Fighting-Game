// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayersInfoManager.generated.h"

UCLASS()
class NARUTO_API APlayersInfoManager : public AActor
{
	GENERATED_BODY()
	
public:	
	APlayersInfoManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(Replicated, VisibleAnywhere, Category = "Info")
	float P1Health;

	UPROPERTY(Replicated, VisibleAnywhere, Category = "Info")
	float P2Health;

	UFUNCTION()
	void SetState(int idx, float Health);

	UFUNCTION()
	float GetState(int idx);

};
