// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AttackStruct.h"
#include "NGameInstance.generated.h"

UCLASS()
class NARUTO_API UNGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
    UNGameInstance();

//MODE...
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameInstance Settings")
	float RoundTime;
	
	// 승리해야하는 라운드..
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameInstance Settings")
	int NumRounds;
};
