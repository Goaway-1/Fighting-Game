#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
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

/** ��/�� �������� ���еǴ� ��Ÿ�� */
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
    /** 2���� ���� */
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

/** ���� ��Ÿ�� */
USTRUCT(Atomic, BlueprintType)
struct FAttackMontageStruct 
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* MT_Attacker;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* MT_Victim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)   // Split�� �ε���
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

//���� ������ datatable
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