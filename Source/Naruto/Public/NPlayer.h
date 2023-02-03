#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AttackStruct.h"
#include "NPlayer.generated.h"

/** Current Player Condition(State) */
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
	EPC_CantMove		UMETA(DisplayName = "CantMove"),	 
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

private:
	class ANCameraManager* CameraManager;
	class ANPlayerController* MainPlayerController;
	class ANPlayerState* MainPlayerState;

public:
	FORCEINLINE ANPlayerController* GetMainController() { return MainPlayerController;}

#pragma region PLAYERCONDITION
public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE EPlayerCondition GetPlayerCondition() { return PlayerCondition; }

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
	EKeyUpDown Key_UD;
	EKeyLeftRight Key_LR;

	/* Jump */
	const double JumpMovementForce = 1000.f;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Movement|Jump")
	bool bIsDoubleJump;

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
	double AP_Distance;								// Distance Between Another Player
	const double ChacraDashForce = 5000.f;			
	const double ChacraDashStopDis = 150.f;			// If below this distance, ChacraDash stop

	UPROPERTY()
	FTimerHandle StopChacraDashHandle;

protected:
	void ChacraDash();
	void StopChacraDash();

	UFUNCTION()
	void AutoChacraDash(float DeltaTime);			// Dash if Player Condition is Dash...
#pragma endregion

#pragma region SIDESTEP
protected:
	UPROPERTY(Replicated, VisibleAnywhere)
	int8 SideStepCnt = 4;

	const int8 SideStepMaxCnt = 4;

	UPROPERTY()
	FTimerHandle SideStepHandle;

	UPROPERTY(EditDefaultsOnly, Category = "SideStep | Particle")
	class UParticleSystem* SideStepParticle = nullptr;    

	UFUNCTION()
	void RecoverSideStepCnt();							// Recover Side Step's Count (Timer)

	/** Start SideStep */
	void SideStep();
	
	UFUNCTION(Client, Reliable)
	void ClientSideStep(FRotator rot);					// For Client Rotation

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSideStep();								// For Server Move & Rot

#pragma endregion

#pragma region WEAPON
protected:
	/* Weapon & AttackManager */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AActor> StarterWeaponClass;

	UPROPERTY(Replicated, VisibleDefaultsOnly, BlueprintReadWrite,Category = "Weapon")
	class ANWeapon* CurrentWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UAttackManager* AttackManager;

	FName WeaponAttachSocketName;

	void SetWeapon();
	void Attack();
public:
	UFUNCTION()
	FORCEINLINE UAttackManager* GetAttackManager() {return AttackManager;}

#pragma endregion

#pragma region Chacra
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UChacraManager* ChacraManager;
	float ChacraPressedTime;							// The time when Chacra pressed
	const float ChacraChargingSec = 0.4f;				// The time it take to Charging
	bool bisCharing = false;							// Check Charging 

	void StartChacra();
	void ChargingChacra();
	void EndChacra();
public:
	UFUNCTION()
	FORCEINLINE UChacraManager* GetChacraManager() { return ChacraManager; }
#pragma endregion

#pragma region CHECK_ANOTHER_ACTOR
protected:
	UPROPERTY(Replicated, VisibleAnywhere)
	ANPlayer* AnotherPlayer;

	double AutoRotDistance = 450.f;			// Can Attack Min Distance
	FVector DirectionVec;

	void SetAnotherPlayer();				// Get Another Player
public:
	FORCEINLINE FVector GetAnotherLocation() { return AnotherPlayer->GetActorLocation(); }
	FORCEINLINE bool IsAnotherPlayer() { return (AnotherPlayer) ? true: false; }
	FORCEINLINE ANPlayer* GetAnotherPlayer() { return AnotherPlayer; }
#pragma endregion

#pragma region MONTAGE
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UMontageManager* MontageManager;

public:
	FORCEINLINE UMontageManager* GetMontageManager() {return MontageManager;}
#pragma endregion

#pragma region BLOCK
private:
	const float BlockDegree = 0.5f;					// can block '-60 ~ 60 degree'
public:
	void PressBlock();
	void ReleaseBlock();
#pragma endregion

#pragma region CUTSCENE
public:
	UFUNCTION(Client, Reliable)
	void ClientPlayScene(bool bisAttacker, int idx = 0);					// Play Scene IN Client
#pragma endregion

#pragma region GRAVITY
protected:
	UPROPERTY()
	FTimerHandle GravityHandle;		          // Set Default Gravity

	const float ResetGravityTime = 0.7f;      // Reset Gravity Time..

	void UpdateGravity();					 // Check Gravity On/Off
public:
	/** Grvaity Value.. */
	float GravityVal = 1.f;
	bool isGravityDone = false;

	void EndGravity();											// Reset Gravity

	/**
	* Player Gravity ON/OFF 
	* @param val       Gravity Value
	* @param bSetAll   Set Gravity to All Player or not 
	*/
	UFUNCTION()
	void SetGravity(float val = 0, bool bSetAll = false);						

protected:
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MultiSetGravity(float val = 0, bool bSetAll = false);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetGravity(float val = 0, bool bSetAll = false);

#pragma endregion

#pragma region NINJA_STAR
public:
	void ThrowStar();
#pragma endregion

#pragma region HEALTH
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UHealthManager* HealthManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	class UDataTable* AttackDataTable;

	FDamageValue* AttackData;

	void DecreasedHealth(int8 DamageSize);						// if Hited
public:
	FORCEINLINE UHealthManager* GetHealthManager() { return HealthManager; }

	/**
	* Play Animation, Decreased Health, Set Condtion when is Player Hited 
	* @param AttackerCondition     Attacker Condition..
	* @param AttackCnt             Attacker Attack Count..
	*/
	void IsHited(EPlayerCondition AttackerCondition, int8 AttackCnt);

	/**
	* Retrun Damage Acount on characteristics
	* @param AttackerCondition     Attacker Condition..
	* @param AttackCnt             Attacker Attack Count..
	* @return					   Damage Acount..
	*/
	int GetDamageValue(EPlayerCondition AttackerCondition, int8 AttackCnt);
#pragma endregion

#pragma region WIDGET
public:
	UFUNCTION(Server, Reliable, WithValidation)
	void UpdateWidget(const EWidgetState state);			// Save state at PlayerState & Update Widget 
#pragma endregion

#pragma region TESTMODE
public:
	/** To Use Player Condition... (FORCE) */
	UPROPERTY(EditAnywhere, Category="TestMode")
	bool bTestMode = false;

	UPROPERTY(EditAnywhere, Category = "TestMode")
	EPlayerCondition TestModePlayerCondition;
#pragma endregion

#pragma region ROUND
public:
	int Score;					
	int CanRecoverCnt;			// number of Count that can be revived

	UPROPERTY()
	FTimerHandle RecoverHandle;

	UFUNCTION()
	void RecoverPlayer();		// Recover Player's Setting.. if player Die
#pragma endregion
};