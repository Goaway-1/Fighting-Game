#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AttackStruct.h"
#include "NPlayer.generated.h"

UENUM(BlueprintType)
enum class EPlayerCondition : uint8 {
	EPC_Idle			UMETA(DisplayName = "Idle"),
	EPC_Hited			UMETA(DisplayName = "Hited"),
	EPC_UpHited			UMETA(DisplayName = "UpHited"),
	EPC_DownHited		UMETA(DisplayName = "DownHited"),
	EPC_AirHited		UMETA(DisplayName = "AirHited"),
	EPC_UpperHited		UMETA(DisplayName = "UpperHited"),
	EPC_Parry			UMETA(DisplayName = "Parry"),
	EPC_Charge			UMETA(DisplayName = "Charge"),
	EPC_Block			UMETA(DisplayName = "Block"),
	EPC_Dash			UMETA(DisplayName = "Dash"),
	EPC_Jump			UMETA(DisplayName = "Jump"),
	EPC_Attack			UMETA(DisplayName = "Attack"),
	EPC_UpAttack		UMETA(DisplayName = "UpAttack"),
	EPC_DownAttack		UMETA(DisplayName = "DownAttack"),
	EPC_UpperAttack		UMETA(DisplayName = "UpperAttack"),
	EPC_AirAttack		UMETA(DisplayName = "AirAttack"),
	EPC_Grap			UMETA(DisplayName = "Grap"),	
	EPC_Skill1			UMETA(DisplayName = "Skill1"),
	EPC_Skill2			UMETA(DisplayName = "Skill2"),
	EPC_CantMove		UMETA(DisplayName = "CantMove"),	//CutScene 
	EPC_Dead			UMETA(DisplayName = "Dead")	
};

UCLASS()
class NARUTO_API ANPlayer : public ACharacter
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;

public:
	ANPlayer();
	virtual void Tick(float DeltaTime) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	/** MainCameraManager class to find */
	UPROPERTY(EditDefaultsOnly, Category = "InitSetting")
	TSubclassOf<AActor> CameraManagerClass;

	class ANCameraManager* CameraManager;

	class ANPlayerController* MainPlayerController;

	class ANPlayerState* MainPlayerState;

	ANCameraManager* TargetCamera;

public:
	UFUNCTION()
	FORCEINLINE ANPlayerController* GetMainController() { return MainPlayerController;}

	FString GetEnumToString(EPlayerCondition value);

protected:
	//UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	//class UPlayerCameraComponent* PlayerCameraComp;
#pragma region PLAYERCONDITION
public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE EPlayerCondition GetPlayerCondition() { return PlayerCondition; }

	// @TODO : ���� ������ ���� �ӽù����̱⿡ �����ʿ�
	//UFUNCTION(BlueprintCallable)
	UFUNCTION(BlueprintCallable,Client, Reliable)
	void SetPlayerCondition(EPlayerCondition NewCondition);		

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetPlayerCondition(EPlayerCondition NewCondition);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsPlayerCondition(EPlayerCondition Condition) { return (PlayerCondition == Condition); }

	/* Check Player Can Input..? */
	bool IsCanMove();

protected:
	UPROPERTY(Replicated,VisibleAnywhere, Category = "Condition")
	EPlayerCondition PlayerCondition;
#pragma endregion

#pragma region MOVE
protected:
	/** The player moves based on the rotation of the CameraManager */
	void MoveForward(float Value);
	void MoveRight(float Value);

	/* Inpressed Keys */
	UPROPERTY(VisibleAnywhere, Category = "Movement")
	EKeyUpDown Key_UD;

	UPROPERTY(VisibleAnywhere, Category = "Movement")
	EKeyLeftRight Key_LR;

	/* Another Player's Distance */
	double AP_Distance;

	/* Jump */
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Movement|Jump")
	bool bIsDoubleJump;

	UPROPERTY(EditDefaultsOnly, Category = "Movement|Jump")
	float JumpMovementForce;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerJump();

	FORCEINLINE void SetKeyUpDown(EKeyUpDown newKey) { Key_UD = newKey; }
	FORCEINLINE void SetKeyLeftRight(EKeyLeftRight newKey) { Key_LR = newKey; }

	FORCEINLINE EKeyUpDown GetKeyUpDown() { return Key_UD; }
	FORCEINLINE EKeyLeftRight GetKeyLeftRight() { return Key_LR; }

public:
	virtual void Jump() override;
	virtual void ResetJumpState() override;

#pragma endregion

#pragma region CHACRADASH
protected:
	double ChacraDashForce = 5000.f;
	double ChacraDashStopDis = 150.f;

	UPROPERTY()
	FTimerHandle StopChacraDashHandle;

protected:
	UFUNCTION()
	void AutoChacraDash(float DeltaTime);

	UFUNCTION()
	void StopChacraDash();

	UFUNCTION()
	void ChacraDash();
#pragma endregion

#pragma region SIDESTEP
protected:
	UPROPERTY(Replicated, VisibleAnywhere)
	int8 SideStepCnt = 4;

	int8 SideStepMaxCnt = 4;

	UPROPERTY()
	FTimerHandle SideStepHandle;

	UPROPERTY(EditDefaultsOnly, Category = "SideStep | Particle")
	class UParticleSystem* SideStepParticle = nullptr;    // Particle.. 

	UFUNCTION()
	void SideStep();

	UFUNCTION()
	void RecoverSideStepCnt();							// Recover Side Step's Count (Timer)
	
	UFUNCTION(Client, Reliable)
	void ClientSideStep(FRotator rot);					// For Client Rotation

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSideStep();								// For Server Move & Rot

#pragma endregion

#pragma region WEAPON
protected:
	/* Weapon */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AActor> StarterWeaponClass;

	UPROPERTY(Replicated, VisibleDefaultsOnly, BlueprintReadWrite,Category = "Weapon")
	class ANWeapon* CurrentWeapon;

	UPROPERTY(VisibleDefaultsOnly, Category = "Weapon")
	FName WeaponAttachSocketName;

	void SetWeapon();

	/** Attack */
	void Attack();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly,  Category = "Attack")
	class UAttackActorComponent* CurAttackComp;

public:
	UFUNCTION()
	FORCEINLINE UAttackActorComponent* GetCurAttackComp() {return CurAttackComp;}

#pragma endregion

#pragma region Chacra
protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Chacra")
	class UChacraActorComponent* CurChacraComp;

	float ChacraPressedTime;							// The time when Chacra pressed
	const float ChacraChargingSec = 0.4f;				// The time it take to Charging

	bool bisCharing = false;							// Check Charging 

	void StartChacra();
	void ChargingChacra();
	void EndChacra();
public:
	UFUNCTION()
	FORCEINLINE UChacraActorComponent* GetCurChacraComp() { return CurChacraComp; }
#pragma endregion

#pragma region CHECK_ANOTHER_ACTOR
protected:
	UPROPERTY(Replicated, VisibleAnywhere)
	ANPlayer* AnotherPlayer;

	double AutoRotDistance = 450.f;			// Can Attack Min Distance
	FVector DirectionVec;

	void SetAnotherPlayer();				// Get Another Playe
public:
	FORCEINLINE FVector GetAnotherLocation() { return AnotherPlayer->GetActorLocation(); }
	FORCEINLINE bool IsAnotherPlayer() { return (AnotherPlayer) ? true: false; }
	FORCEINLINE ANPlayer* GetAnotherPlayer() { return AnotherPlayer; }
#pragma endregion

#pragma region MONTAGE
protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	class UMontageManager* MontageManager;

public:
	UFUNCTION()
	FORCEINLINE UMontageManager* GetMontageManager() {return MontageManager;}
#pragma endregion

#pragma region BLOCK
private:
	const float BlockDegree = 0.5f;   // can block '-60 ~ 60 degree'
public:
	UFUNCTION()
	void PressBlock();

	UFUNCTION()
	void ReleaseBlock();
#pragma endregion

#pragma region CUTSCENE
public:
	UFUNCTION(Client, Reliable)
	void ClientPlayScene(bool bisAttacker, int idx = 0);					// Play Scene IN Client
#pragma endregion

#pragma region AIRATTACK
protected:
	UPROPERTY()
	FTimerHandle GravityHandle;		          // Set Default Gravity

	const float ResetGravityTime = 0.7f;    // Reset Time..

public:
	/** Player Gravity ON */
	UFUNCTION()
	void SetGravity(float val = 0);						

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MultiSetGravity(float val = 0);						

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetGravity(float val = 0);						

	/** All Players Gravity ON */
	UFUNCTION()
	void SetAllGravity(float val = 0);						

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MultiSetAllGravity(float val = 0);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetAllGravity(float val = 0);         

	/** Grvaity Value.. */
	float GravityVal = 1.f;
	bool isGravityDone = false;

	UFUNCTION()
	void EndGravity();											// Gravity OFF

	void UpdateGravity();										// for Tick...
#pragma endregion

#pragma region NINJA_STAR
public:
	UFUNCTION()
	void ThrowStar();
#pragma endregion

#pragma region HEALTH
protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	class UHealthManager* HealthManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	class UDataTable* AttackDataTable;

	FDamageValue* AttackData;

	UFUNCTION()
	void DecreasedHealth(int8 DamageSize);						// if Hited

public:
	UFUNCTION()
	FORCEINLINE UHealthManager* GetHealthManager() { return HealthManager; }

	UFUNCTION()
	void IsHited(EPlayerCondition AttackerCondition, int8 AttackCnt);

	UFUNCTION()
	int GetDamageValue(EPlayerCondition AttackerCondition, int8 AttackCnt);
#pragma endregion

#pragma region WIDGET
public:
	UFUNCTION(Server, Reliable, WithValidation)
	void UpdateWidget(const EWidgetState state);			// Save state at PlayerState & Update Widget 
#pragma endregion

#pragma region TESTMODE
public:
	UPROPERTY(EditAnywhere, Category="TestMode")
	bool bTestMode = false;

	UPROPERTY(EditAnywhere, Category = "TestMode")
	EPlayerCondition TestModePlayerCondition;
#pragma endregion

#pragma region ROUND
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Round")
	int Score;					

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Round")
	int CanRecoverCnt;			// Can Recover Count..

	UPROPERTY()
	FTimerHandle RecoverHandle;

	UFUNCTION()
	void RecoverPlayer();		// Recover Player's Setting.. if player Die
#pragma endregion
};