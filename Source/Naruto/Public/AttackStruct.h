// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AttackStruct.generated.h"

UENUM(BlueprintType)
enum class EKeyUpDown : uint8 {
	EKUD_Up				UMETA(DisplayName = "Up"),
	EKUD_Down			UMETA(DisplayName = "Down"),

	EKUD_Default		UMETA(DisplayName = "Default")
};

UENUM(BlueprintType)
enum class EKeyLeftRight : uint8 {
	EKLR_Right			UMETA(DisplayName = "Right"),
	EKLR_Left			UMETA(DisplayName = "Left"),

	EKLR_Default		UMETA(DisplayName = "Default")
};

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
struct FAttackSplitMontageStruct
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* MTUP_Attacker;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* MTUP_Victim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* MTDOWN_Attacker;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* MTDOWN_Victim;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)   // Split«“ ¿Œµ¶Ω∫
	int splitIdx;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAttackSplitMontageStruct AttackSplit;
};
