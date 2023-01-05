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

/** 상/하 공격으로 구분되는 몽타주 */
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
struct FChacraAttackMontageStruct
{
	GENERATED_BODY()
public:
    /** 2개로 고정 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class UAnimMontage*> MTChacra_Attacker;
};

USTRUCT(Atomic, BlueprintType)
struct FChacraSkill
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UMediaSource* MS_SkillMedia;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MediaLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* MT_AttackerSkillEnd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* MT_VitcimSkillEnd;
};

/** 공격 몽타주 */
USTRUCT(Atomic, BlueprintType)
struct FAttackMontageStruct
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* MT_Attacker;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* MT_Victim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)   // Split할 인덱스
	int splitIdx;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAttackSplitMontageStruct AttackSplit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FChacraAttackMontageStruct ChacraAttack;

	/** Hold Attack */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* MT_GrapAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* MT_GrapVictim;

	/** Dash */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* MT_ChacraDash;

	/** Skill Media */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FChacraSkill> MS_Skill;
};
