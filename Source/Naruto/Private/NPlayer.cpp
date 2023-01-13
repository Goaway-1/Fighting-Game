#include "NPlayer.h"
#include "NPlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NCameraManager.h"
#include "NGameMode.h"
#include "NWeapon.h"
#include "HealthManager.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"
#include "AttackActorComponent.h"
#include "ChacraActorComponent.h"
#include "MontageManager.h"
#include "NPlayerState.h"
#include "DrawDebugHelpers.h"

ANPlayer::ANPlayer() {
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	/** Configure character Rotation */
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationPitch = false;

	/** Configure character movement */
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 2000.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = 800.f;

	/* Jump Setting */
	JumpMaxCount = 2;
	JumpMovementForce = 1000.f;
	GetCharacterMovement()->JumpZVelocity = 1400.f;
	GetCharacterMovement()->AirControl = 0.f;
	GetCharacterMovement()->AirControlBoostMultiplier = 0.f;
	GetCharacterMovement()->AirControlBoostVelocityThreshold = 0.f;
	bIsDoubleJump = false;

	/* Impressed Keys */
	SetKeyUpDown(EKeyUpDown::EKUD_Default);
	SetKeyLeftRight(EKeyLeftRight::EKLR_Default);

	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

	/* Weapon */
	WeaponAttachSocketName = "WeaponSocket";

	/* Attack & Chacra Component */ 
	CurAttackComp = CreateDefaultSubobject<UAttackActorComponent>(TEXT("AttackComponent"));
	CurChacraComp = CreateDefaultSubobject<UChacraActorComponent>(TEXT("ChacraComponent"));
	MontageManager = CreateDefaultSubobject<UMontageManager>(TEXT("MontageManager"));
	HealthManager = CreateDefaultSubobject<UHealthManager>(TEXT("HealthManager"));

	/* Set Condition */
	SetPlayerCondition(EPlayerCondition::EPC_Idle);
}
void ANPlayer::BeginPlay() {
	Super::BeginPlay();
	
	/** Find MainCameraManager & Set */
	if (CameraManagerClass) {
		TargetCamera = Cast<ANCameraManager>(UGameplayStatics::GetActorOfClass(this, ANCameraManager::StaticClass()));

		if (TargetCamera) {
			CameraManager = TargetCamera;
			MainPlayerController = Cast<ANPlayerController>(GetController());
			if (MainPlayerController) {
				MainPlayerController->bAutoManageActiveCameraTarget = false;
				MainPlayerController->ClientSetViewTarget(CameraManager);
			}
		}
		else GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Error! MainCameraManager does not exist in the world!"));
	}

	if (CameraManager) CameraManager->SetPlayer(this);
	
	/** Weapon */
	SetWeapon();

	/* Attack */
	CurAttackComp->SetInit(this);
	MontageManager->SetInit(this, GetMesh()->GetAnimInstance());
}
void ANPlayer::PossessedBy(AController* NewController) {
	Super::PossessedBy(NewController);

	if (!MainPlayerState) MainPlayerState = Cast<ANPlayerState>(GetPlayerState());
}
void ANPlayer::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	// @TODO : 지워야돼
	GetHealthManager()->SetDecreaseHealth(0.3f);
	if (MainPlayerState) MainPlayerState->SetHealth(GetHealthManager()->GetCurrentHealth());

	// 지울거얌 : 강제로 상태 만들었음
	if (bTestMode) {
		SetPlayerCondition(TestModePlayerCondition);
		return;
	}

	/** Setting the rotation of the controller according to the rotation of the CameraManager */
	if (CameraManager && HasAuthority()) {
		FRotator NRot = FRotator(CameraManager->GetActorRotation().Pitch, CameraManager->GetActorRotation().Yaw, GetController()->GetControlRotation().Roll);
		
		if (MainPlayerController) MainPlayerController->ClientSetRotation(NRot);
		else MainPlayerController = Cast<ANPlayerController>(GetController());
	}

	/** Get Another Player */
	SetAnotherPlayer();

	/** Active Chacra Dash */
	AutoChacraDash(DeltaTime);

	/** ON& OFF Gravitiy */
	if (!bIsGravityHandling && (IsPlayerCondition(EPlayerCondition::EPC_AirAttack) || IsPlayerCondition(EPlayerCondition::EPC_AirHited))) {
		SetGravity(0.f);
		GetMovementComponent()->StopMovementImmediately();

		bIsGravityHandling = true;
		//FTimerDelegate TimerDel;
		//TimerDel.BindUFunction(this, FName("SetGravity"), 1.f);
		//GetWorld()->GetTimerManager().ClearTimer(GravityHandle);
		////GetWorld()->GetTimerManager().SetTimer(GravityHandle, TimerDel, ResetGravityTime, false);
		//GetWorld()->GetTimerManager().SetTimer(GravityHandle, TimerDel, 2.f, false);
	}
}
void ANPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed,this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released,this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ANPlayer::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ANPlayer::MoveRight);

	// Attack
	PlayerInputComponent->BindAction("Attack", IE_Pressed,this, &ANPlayer::Attack);
	PlayerInputComponent->BindAction("SideStep", IE_Pressed,this, &ANPlayer::SideStep);
	PlayerInputComponent->BindAction("Block", IE_Pressed,this, &ANPlayer::PressBlock);
	PlayerInputComponent->BindAction("Block", IE_Released,this, &ANPlayer::ReleaseBlock);

	// Chacra
	PlayerInputComponent->BindAction("Chacra", IE_Pressed,this, &ANPlayer::StartChacra);
	PlayerInputComponent->BindAction("Chacra", IE_Repeat,this, &ANPlayer::ChargingChacra);
	PlayerInputComponent->BindAction("Chacra", IE_Released,this, &ANPlayer::EndChacra);

	// Ninja Star
	PlayerInputComponent->BindAction("NinjaStar", IE_Pressed,this, &ANPlayer::ThrowStar);
}
void ANPlayer::SetAnotherPlayer() {
	if (!AnotherPlayer) {
		for (auto x : GetWorld()->GetGameState()->PlayerArray) {
			if (this != x->GetPawn()) {
				AnotherPlayer = Cast<ANPlayer>(x->GetPawn());
				break;
			}
		}
	}
	else {
		/** Set IsInRange & DirectionVector (this to AnotherActor)*/
		AP_Distance = (GetActorLocation() - AnotherPlayer->GetActorLocation()).Size();

		DirectionVec = (GetAnotherLocation() - GetActorLocation()).GetSafeNormal();
		DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + (DirectionVec * 100), FColor::Red, false, 0, 0, 5);
	}
}
void ANPlayer::MoveForward(float Value) {
	if (Value > 0) SetKeyUpDown(EKeyUpDown::EKUD_Up);
	else if (Value < 0) SetKeyUpDown(EKeyUpDown::EKUD_Down);
	else SetKeyUpDown(EKeyUpDown::EKUD_Default);

	if(!IsCanMove()) return;

	FRotator Rot = FRotator(0.f, GetControlRotation().Yaw, 0.f);
	AddMovementInput(UKismetMathLibrary::GetForwardVector(Rot), Value);
}
void ANPlayer::MoveRight(float Value) {
	if (Value > 0) SetKeyLeftRight(EKeyLeftRight::EKLR_Right);
	else if (Value < 0) SetKeyLeftRight(EKeyLeftRight::EKLR_Left);
	else SetKeyLeftRight(EKeyLeftRight::EKLR_Default);
	
	if (!IsCanMove()) return;

	FRotator Rot = FRotator(0.f, GetControlRotation().Yaw, 0.f);
	AddMovementInput(UKismetMathLibrary::GetRightVector(Rot), Value);
}
bool ANPlayer::IsCanMove() {
	if(IsPlayerCondition(EPlayerCondition::EPC_Dead) || IsPlayerCondition(EPlayerCondition::EPC_CantMove) || IsPlayerCondition(EPlayerCondition::EPC_Block) || IsPlayerCondition(EPlayerCondition::EPC_Attack) || IsPlayerCondition(EPlayerCondition::EPC_Dash) || IsPlayerCondition(EPlayerCondition::EPC_Hited)) return false;
	else return true;
}
void ANPlayer::Jump() {
	if (!IsCanMove()) return;

	if (CurChacraComp->GetChacraCnt() > 0) {
		ChacraDash();
	}
	else {
		if (!HasAuthority()) {
			ServerJump();
		}

		if (JumpCurrentCount++ < 2) {
			/** for Animation */
			if (JumpCurrentCount == 2) {
				GetMovementComponent()->StopMovementImmediately();
				bIsDoubleJump = true;
			}
			else bIsDoubleJump = false;

			FVector ForceVec = (GetActorUpVector() * (JumpMovementForce - GetVelocity().Size())) + (GetActorForwardVector() * (JumpMovementForce - GetVelocity().Size()));
			ForceVec.Z = 1400.f;
			LaunchCharacter(ForceVec, false, false);
		}
	}
}
void ANPlayer::ServerJump_Implementation() {
	JumpCurrentCount++;
	Jump();
}
bool ANPlayer::ServerJump_Validate() {
	return true;
}
void ANPlayer::ResetJumpState() {
	Super::ResetJumpState();

	if (GetCharacterMovement() && !GetCharacterMovement()->IsFalling()) bIsDoubleJump = false;
}
void ANPlayer::SetWeapon() {
	if (HasAuthority()) {
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		if (StarterWeaponClass) CurrentWeapon = GetWorld()->SpawnActor<ANWeapon>(StarterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (CurrentWeapon) {
			CurrentWeapon->SetOwner(this);
			CurrentWeapon->SetWeaponRandom();
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
		}
	}
}
void ANPlayer::Attack() {
	if (IsPlayerCondition(EPlayerCondition::EPC_Dead) || IsPlayerCondition(EPlayerCondition::EPC_CantMove) || IsPlayerCondition(EPlayerCondition::EPC_Dash) || IsPlayerCondition(EPlayerCondition::EPC_Hited)) return;

	if(IsLocallyControlled()) CurAttackComp->DefaultAttack_KeyDown(GetKeyUpDown());
}
void ANPlayer::IsHited() {
	/** if blocking */
	if (IsPlayerCondition(EPlayerCondition::EPC_Block) && AnotherPlayer->IsPlayerCondition(EPlayerCondition::EPC_Attack)) {
		// Can Block -60 ~ 60 Degree
		float Inner = GetDotProductTo(AnotherPlayer);

		if (Inner > BlockDegree) {
			GetMontageManager()->PlayNetworkMontage(AnotherPlayer->GetMontageManager()->GetActionMontage().MT_BlockHited, 1.f, GetPlayerCondition());
			UE_LOG(LogTemp, Warning, TEXT("Block Successed!!"));
			return;
		}
		else UE_LOG(LogTemp, Warning, TEXT("Block failed!"));
	}
	
	/** Decreased Health & UI */
	if (GetHealthManager()->SetDecreaseHealth(10.f)) {
		GetCurAttackComp()->RotateToActor();
		UE_LOG(LogTemp, Warning, TEXT("Player is Died"));
		SetPlayerCondition(EPlayerCondition::EPC_Dead);
		return;
	}
	if (MainPlayerState) MainPlayerState->SetHealth(GetHealthManager()->GetCurrentHealth());

	SetPlayerCondition(EPlayerCondition::EPC_Hited);
	if (AnotherPlayer->IsPlayerCondition(EPlayerCondition::EPC_Skill1) || AnotherPlayer->IsPlayerCondition(EPlayerCondition::EPC_Skill2)) {
		
		/** Play CutScene if Skill */
		UE_LOG(LogTemp, Warning, TEXT("%s Skill %s"), *AnotherPlayer->GetName(), *this->GetName());
		int SkillIdx = (AnotherPlayer->IsPlayerCondition(EPlayerCondition::EPC_Skill1)) ? 0: 1;
		ClientPlayScene(false, SkillIdx);
		AnotherPlayer->ClientPlayScene(true, SkillIdx);
	}
	else if (AnotherPlayer->IsPlayerCondition(EPlayerCondition::EPC_Grap)) {
		UE_LOG(LogTemp, Warning, TEXT("%s Grap %s"), *AnotherPlayer->GetName(), *this->GetName());

		/** Set Victim  Rotate & Location */
		UAnimMontage* mon = AnotherPlayer->GetMontageManager()->GetActionMontage().MT_GrapVictim;
		GetMontageManager()->PlayNetworkMontage(mon, 1.f, GetPlayerCondition());
		GetCurAttackComp()->RotateToActor();

		FVector forceVec = AnotherPlayer->GetActorLocation() + (AnotherPlayer->GetActorForwardVector() * 100.f);
		SetActorLocation(forceVec);

		AnotherPlayer->GetCurAttackComp()->bGrapHited = true;
	}
	else  {
		if (AnotherPlayer->IsPlayerCondition(EPlayerCondition::EPC_AirAttack)) {					/** 공중 공격 */
			UE_LOG(LogTemp, Warning, TEXT("Air Vitcim!"));
			SetPlayerCondition(EPlayerCondition::EPC_AirHited);
			UAnimMontage* mon = AnotherPlayer->GetMontageManager()->GetActionMontage().MT_JumpVictim;
			GetMontageManager()->PlayNetworkMontage(mon, 1.f, GetPlayerCondition());

			// Set ON Gravity & GO DOWN if Last Attack Hited
			if(AnotherPlayer->GetCurAttackComp()->GetComboCnt() == 5) {
				//LaunchCharacter(GetActorUpVector() * - 1000.f,false,false);
				SetGravity(1.f);
			}
			else bIsGravityHandling = false;
		}
		else if (AnotherPlayer->IsPlayerCondition(EPlayerCondition::EPC_UpperAttack)) {				/** 공중으로 날려! */
			UE_LOG(LogTemp, Warning, TEXT("Upper Vitcim!"));
			SetPlayerCondition(EPlayerCondition::EPC_UpperHited);

			// Launch Another Characer
			FVector ForceVec = (GetActorForwardVector() * -500.f) + FVector(0.f, 0.f, 2000.f);
			LaunchCharacter(ForceVec, false, false);
			
			bIsGravityHandling = false;
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Just Vitcim!"));
			UAnimMontage* mon = AnotherPlayer->GetMontageManager()->GetActionMontage().MT_Victim;
			GetMontageManager()->PlayNetworkMontage(mon, 1.f, GetPlayerCondition(), AnotherPlayer->GetCurAttackComp()->GetComboCnt());
		}

		/** Set Camera & Player Rotate */
		GetCurAttackComp()->RotateToActor();
		if (AnotherPlayer->GetCurAttackComp()->GetComboCnt() >= 2) TargetCamera->SetAttackView();
	}
}
void ANPlayer::PressBlock() { 
	if(!GetMovementComponent()->IsFalling()) SetPlayerCondition(EPlayerCondition::EPC_Block);
}
void ANPlayer::ReleaseBlock() { 
	if (IsPlayerCondition(EPlayerCondition::EPC_Block)) SetPlayerCondition(EPlayerCondition::EPC_Idle);
}
void ANPlayer::ClientPlayScene_Implementation(bool bisAttacker, int idx) {
	GetMontageManager()->StopNetworkMontage();
	SetPlayerCondition(EPlayerCondition::EPC_CantMove);
	if (bisAttacker) {
		GetMainController()->PlayCutScene(GetMontageManager()->GetActionMontage().MS_Skill[idx].MS_SkillMedia, GetMontageManager()->GetActionMontage().MS_Skill[idx].MT_AttackerSkillEnd, GetMontageManager()->GetActionMontage().MS_Skill[idx].MediaLength);
	}
	else {
		GetMainController()->PlayCutScene(GetMontageManager()->GetActionMontage().MS_Skill[idx].MS_SkillMedia, GetMontageManager()->GetActionMontage().MS_Skill[idx].MT_VitcimSkillEnd, GetMontageManager()->GetActionMontage().MS_Skill[idx].MediaLength);
	}
}
void ANPlayer::SetPlayerCondition_Implementation(EPlayerCondition NewCondition) {
	/*if (!HasAuthority()) ServerSetPlayerCondition(NewCondition);
	else PlayerCondition = NewCondition;*/
	ServerSetPlayerCondition(NewCondition);
}
void ANPlayer::ServerSetPlayerCondition_Implementation(EPlayerCondition NewCondition) {
	//SetPlayerCondition(NewCondition);
	PlayerCondition = NewCondition;
}
bool ANPlayer::ServerSetPlayerCondition_Validate(EPlayerCondition NewCondition) {
	return true;
}
void ANPlayer::StartChacra() {
	ChacraPressedTime = FDateTime::Now().GetTicks();
	bisCharing = false;
}
void ANPlayer::ChargingChacra() {
	if (!IsPlayerCondition(EPlayerCondition::EPC_Idle)) {
		EndChacra();
		return;
	}

	/** ChacraCharging in few Seconds */
	int64 HoldingTime = ChacraPressedTime / 10000;
	if (HoldingTime >= ChacraChargingSec) {
		CurChacraComp->ChargingChacra();
		bisCharing = true;
	}
}
void ANPlayer::EndChacra() {
	if(bisCharing) return;

	/** Try to Set Chacra */
	CurChacraComp->UseChacra();
}
void ANPlayer::ChacraDash() {
	if (AnotherPlayer) {
		UE_LOG(LogTemp, Warning, TEXT("Chacra Dash to %s"), *AnotherPlayer->GetName());

		if (!HasAuthority()) ServerChacraDash();
		else {
			SetPlayerCondition(EPlayerCondition::EPC_Dash);
			CurChacraComp->ResetChacraCnt();
			CurAttackComp->ResetAll();

			// Animation
			GetMontageManager()->PlayNetworkMontage(GetMontageManager()->GetActionMontage().MT_ChacraDash, 0.f, GetPlayerCondition());

			// Reset Timer
			GetWorld()->GetTimerManager().ClearTimer(StopChacraDashHandle);
			GetWorld()->GetTimerManager().SetTimer(StopChacraDashHandle, this, &ANPlayer::StopChacraDash, 1.5f, false);
		}
	}
}
void ANPlayer::ServerChacraDash_Implementation() {
	ChacraDash();
}
bool ANPlayer::ServerChacraDash_Validate() {
	return true;
}
void ANPlayer::AutoChacraDash(float DeltaTime) {
	if (IsPlayerCondition(EPlayerCondition::EPC_Dash)) {
		if (AP_Distance < ChacraDashStopDis) {
			StopChacraDash();
			
			// Set Player Condition if Crash 'in Air'
			if (GetMovementComponent()->IsFalling()) {
				UE_LOG(LogTemp, Warning, TEXT("is Working!"));
				SetPlayerCondition(EPlayerCondition::EPC_AirAttack);
				AnotherPlayer->SetPlayerCondition(EPlayerCondition::EPC_AirHited);

				// Set Location (Force)
				FVector TmpVec = AnotherPlayer->GetActorLocation() + (AnotherPlayer->GetActorForwardVector() * 100);
				SetActorLocation(TmpVec);
				bIsGravityHandling = false;
			}
			return;
		}

		// Set Rotation
		FRotator RotateVal = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), GetAnotherLocation());
		RotateVal.Roll = GetActorRotation().Roll;
		RotateVal.Pitch = GetActorRotation().Pitch;
		SetActorRotation(RotateVal);

		// Set Location
		FVector Loc = GetActorLocation();
		Loc += (AnotherPlayer->GetActorLocation() - Loc).GetSafeNormal() * ChacraDashForce * DeltaTime;
		SetActorLocation(Loc);
	}
}
void ANPlayer::StopChacraDash() {
	SetPlayerCondition(EPlayerCondition::EPC_Idle);
	GetMontageManager()->StopNetworkMontage();
}
void ANPlayer::SideStep() {
	//if (!IsPlayerCondition(EPlayerCondition::EPC_Hited) || !AnotherPlayer || SideStepCnt <= 0) return;
	if (!AnotherPlayer || SideStepCnt <= 0) return;

	if(!HasAuthority()){
		ServerSideStep();
	}
	else{
		FVector VecVal = GetAnotherLocation() - (AnotherPlayer->GetActorForwardVector() * 100.f);
		SetActorLocation(VecVal);

		FRotator RotateVal = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), GetAnotherLocation());
		RotateVal.Roll = GetActorRotation().Roll;
		RotateVal.Pitch = GetActorRotation().Pitch;
		SetActorRotation(RotateVal);
		ClientSideStep(RotateVal);
		SideStepCnt--;

		GetWorld()->GetTimerManager().ClearTimer(SideStepHandle);
		GetWorld()->GetTimerManager().SetTimer(SideStepHandle, this, &ANPlayer::RecoverSideStepCnt, 3.f, false);
	}
}
void ANPlayer::ClientSideStep_Implementation(FRotator rot) {
	SetActorRotation(rot);
}
void ANPlayer::ServerSideStep_Implementation() {
	SideStep();
}
bool ANPlayer::ServerSideStep_Validate() {
	return true;
}
void ANPlayer::RecoverSideStepCnt() {
	if (++SideStepCnt < SideStepMaxCnt) {
		UE_LOG(LogTemp, Warning, TEXT("SideStepCnt %d"), SideStepCnt);
		GetWorld()->GetTimerManager().ClearTimer(SideStepHandle);
		GetWorld()->GetTimerManager().SetTimer(SideStepHandle, this, &ANPlayer::RecoverSideStepCnt, 3.f, false);
	}
}
void ANPlayer::SetGravity_Implementation(float val) {
	/*if(!HasAuthority()) SetServerGravity(val);
	
	if (GetCharacterMovement()->GravityScale != val) {
		GetCharacterMovement()->GravityScale = val;
		SetPlayerCondition(EPlayerCondition::EPC_Idle);
	}*/
	SetServerGravity(val);
}
void ANPlayer::SetServerGravity_Implementation(float val) {
	if (GetCharacterMovement()->GravityScale != val) {
		GetCharacterMovement()->GravityScale = val;
		SetPlayerCondition(EPlayerCondition::EPC_Idle);
	}
	
	//SetGravity(val);
}
bool ANPlayer::SetServerGravity_Validate(float val) {
	return true;
}
void ANPlayer::ThrowStar() {
	bool bIsChacraThrow = false;
	if (GetCurChacraComp()->GetChacraCnt() >= 1) {
		GetCurChacraComp()->ResetChacraCnt();
		bIsChacraThrow = true;
	}
	CurAttackComp->ThrowNinjaStar(bIsChacraThrow);
}
void ANPlayer::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANPlayer, CurrentWeapon);
	DOREPLIFETIME(ANPlayer, bIsDoubleJump);
	DOREPLIFETIME(ANPlayer, PlayerCondition);
	DOREPLIFETIME(ANPlayer, SideStepCnt);
	DOREPLIFETIME(ANPlayer, AnotherPlayer);
	DOREPLIFETIME(ANPlayer, bIsGravityHandling);
}