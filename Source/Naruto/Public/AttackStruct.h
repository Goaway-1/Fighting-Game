// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AttackStruct.generated.h"

UCLASS()
class NARUTO_API AAttackStruct : public AActor
{
	GENERATED_BODY()
	
public:	
	AAttackStruct();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};

USTRUCT(Atomic, BlueprintType)
struct FAttackMontageStruct
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* MT_Attacker;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* MT_Victim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ComboCnt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float distance;
};