#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "AttackStruct.generated.h"

/*
* Only Enum & Struct Class..
*/

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

	virtual void BeginPlay() override;
protected:
	virtual void Tick(float DeltaTime) override;

};

/** Animation Struct of Extended Attacks (UP/DOWN) */
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

/** Animation Struct of Skill Initial Animation */
USTRUCT(Atomic, BlueprintType)
struct FChacraAttackMontageStruct 
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class UAnimMontage*> MTChacra_Attacker;
};

/** Animation Struct of ChacraSkills */
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

/** Animation Struct of Action */
USTRUCT(Atomic, BlueprintType)
struct FAttackMontageStruct 
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* MT_Attacker;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* MT_Victim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)   // Split count of Extend Attack
	int splitIdx;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAttackSplitMontageStruct AttackSplit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FChacraAttackMontageStruct ChacraAttack;

	/** Grap Attack */
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

	/** Jumped Attack & Victim */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* MT_JumpAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* MT_JumpVictim;

	/** Jump Hited End */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* MT_JumpVictimEnd;

	/** Block Hited */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* MT_BlockHited;
};

/** Damage Data Struct of Attack (Table) */
USTRUCT(BlueprintType)
struct FDamageValue : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Ak = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int	LastAk = 5;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int	ExtensionAk = 5;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int	ExtensionLastAk = 7;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int	GrapAk = 6;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int	AirAk = 3;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int	AirLastAk = 5;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int	SkillAk1 = 15;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int	SkillAk2 = 25;
};

/** Particle Struct of Chacra */
USTRUCT(Atomic, BlueprintType)
struct FParticles 
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UParticleSystem* ChacraCharging;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UParticleSystem* ChacraActive1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UParticleSystem* ChacraActive2;
};