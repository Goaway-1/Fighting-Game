#include "NPlayer.h"
#include "NPlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NCameraManager.h"
#include "NGameMode.h"
#include "NWeapon.h"
#include "AttackStruct.h"
#include "NGameInstance.h"
#include "HealthManager.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"
#include "AttackActorComponent.h"
#include "ChacraActorComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MontageManager.h"
#include "NPlayerState.h"
#include "PlayersInfoManager.h"
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

	/** Round Value.. */
	Score = 0;
	CanRecoverCnt = 1;
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

	/** Get PlayerState */
	if (!MainPlayerState) MainPlayerState = Cast<ANPlayerState>(GetPlayerState());
}
void ANPlayer::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	// ����ž� : ������ ���� �������
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

	/** Set Gravity.. */
	UpdateGravity();
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

// delete...
FString ANPlayer::GetEnumToString(EPlayerCondition value){		
	switch (value)
	{
	case EPlayerCondition::EPC_Idle:
		return "EPC_Idle";
	case EPlayerCondition::EPC_Hited:
		return "EPC_Hited";
	case EPlayerCondition::EPC_AirHited:
		return "EPC_AirHited";
	case EPlayerCondition::EPC_UpperHited:
		return "EPC_UpperHited";
	case EPlayerCondition::EPC_Parry:
		return "EPC_Parry";
	case EPlayerCondition::EPC_Charge:
		return "EPC_Charge";
	case EPlayerCondition::EPC_Block:
		return "EPC_Block";
	case EPlayerCondition::EPC_Dash:
		return "EPC_Dash";
	case EPlayerCondition::EPC_Jump:
		return "EPC_Jump";
	case EPlayerCondition::EPC_Attack:
		return "EPC_Attack";
	case EPlayerCondition::EPC_UpperAttack:
		return "EPC_UpperAttack";
	case EPlayerCondition::EPC_AirAttack:
		return "EPC_AirAttack";
	case EPlayerCondition::EPC_Grap:
		return "EPC_Grap";
	case EPlayerCondition::EPC_Skill1:
		return "EPC_Skill1";
	case EPlayerCondition::EPC_Skill2:
		return "EPC_Skill2";
	case EPlayerCondition::EPC_CantMove:
		return "EPC_CantMove";
	case EPlayerCondition::EPC_Dead:
		return "EPC_Dead";
	default:
		return "EPC_Idle";
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
	if(IsPlayerCondition(EPlayerCondition::EPC_Charge) || IsPlayerCondition(EPlayerCondition::EPC_Dead) || IsPlayerCondition(EPlayerCondition::EPC_CantMove) || IsPlayerCondition(EPlayerCondition::EPC_Block) || IsPlayerCondition(EPlayerCondition::EPC_Attack) || IsPlayerCondition(EPlayerCondition::EPC_Dash) || IsPlayerCondition(EPlayerCondition::EPC_Hited)) return false;
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
	if(!MainPlayerController->GetIsRoundStart()) return;    // Round 시작까지 대기

	if(IsLocallyControlled()) CurAttackComp->DefaultAttack_KeyDown(GetKeyUpDown());
}
void ANPlayer::IsHited(EPlayerCondition AttackerCondition, int8 AttackCnt) {
	/** if blocking */
	if (IsPlayerCondition(EPlayerCondition::EPC_Block) && AttackerCondition == EPlayerCondition::EPC_Attack) {
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
	DecreasedHealth(GetDamageValue(AttackerCondition, AttackCnt));
	
	/** Check Is Dead? */
	if (GetHealthManager()->GetIsDead()) {
		UE_LOG(LogTemp, Warning, TEXT("Player is Died"));
		SetPlayerCondition(EPlayerCondition::EPC_Dead);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		/** Add Score */
		AnotherPlayer->Score++;
		AnotherPlayer->UpdateWidget(EWidgetState::EWS_Score);

		/** Reset Round & Player Condition */
		if (CanRecoverCnt-- > 0) {
			GetMainController()->RequestRoundEnd();
			GetWorld()->GetTimerManager().ClearTimer(RecoverHandle);
			GetWorld()->GetTimerManager().SetTimer(RecoverHandle, this, &ANPlayer::RecoverPlayer, 3.f, false);
		}
		else {
			GetMainController()->RequestGameOver();
		}
		return;
	}

	SetPlayerCondition(EPlayerCondition::EPC_Hited);
	if (AttackerCondition == EPlayerCondition::EPC_Skill1 || AttackerCondition == EPlayerCondition::EPC_Skill2) {
		
		/** Play CutScene if Skill */
		UE_LOG(LogTemp, Warning, TEXT("%s Skill %s"), *AnotherPlayer->GetName(), *this->GetName());
		int SkillIdx = (AttackerCondition == EPlayerCondition::EPC_Skill1) ? 0: 1;
		ClientPlayScene(false, SkillIdx);
		AnotherPlayer->ClientPlayScene(true, SkillIdx);
	}
	else if (AttackerCondition == EPlayerCondition::EPC_Grap) {
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
		if (GetMovementComponent()->IsFalling()) {
			SetPlayerCondition(EPlayerCondition::EPC_AirHited);
			UAnimMontage* mon;

			// Set ON Gravity & GO DOWN if Last Attack Hited
			if(AttackCnt == 6) {
				LaunchCharacter(GetActorUpVector() * - 1000.f,false,false);
				SetGravity(1.f);
				mon = AnotherPlayer->GetMontageManager()->GetActionMontage().MT_JumpVictimEnd;
			}
			else {
				LaunchCharacter(GetActorForwardVector() * -100.f, false, false);
				SetGravity(0.f);
				mon = AnotherPlayer->GetMontageManager()->GetActionMontage().MT_JumpVictim;
			}

			GetMontageManager()->PlayNetworkMontage(mon, 1.f, GetPlayerCondition());
		}
		else if (AttackerCondition == EPlayerCondition::EPC_UpperAttack) {				/** �������� ����! */
			UE_LOG(LogTemp, Warning, TEXT("Upper Vitcim!"));
			SetPlayerCondition(EPlayerCondition::EPC_UpperHited);

			// Launch Another Characer
			FVector ForceVec = (GetActorForwardVector() * -500.f) + FVector(0.f, 0.f, 2000.f);
			LaunchCharacter(ForceVec, false, false);
		}
		else {
			UAnimMontage* mon;
			if (AttackerCondition == EPlayerCondition::EPC_UpAttack) mon = AnotherPlayer->GetMontageManager()->GetActionMontage().AttackSplit.MTUP_Victim;
			else if (AttackerCondition == EPlayerCondition::EPC_DownAttack) mon = AnotherPlayer->GetMontageManager()->GetActionMontage().AttackSplit.MTDOWN_Victim;
			else mon = AnotherPlayer->GetMontageManager()->GetActionMontage().MT_Victim;

			GetMontageManager()->PlayNetworkMontage(mon, 1.f, EPlayerCondition::EPC_Hited, AttackCnt);
		}

		/** Set Camera & Player Rotate */
		GetCurAttackComp()->RotateToActor();
		if (AttackCnt >= 2) TargetCamera->SetAttackView();
	}
}
void ANPlayer::RecoverPlayer() {
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SetPlayerCondition(EPlayerCondition::EPC_Idle);
	GetHealthManager()->RecoverHealth();
	GetCurChacraComp()->RecoverChacra();

	UpdateWidget(EWidgetState::EWS_Health);
	UpdateWidget(EWidgetState::EWS_Chacra);
}
int ANPlayer::GetDamageValue(EPlayerCondition AttackerCondition, int8 AttackCnt) {
	if(!AttackData) AttackData = AttackDataTable->FindRow<FDamageValue>(FName("Value"), FString(""));

	if (AttackerCondition == EPlayerCondition::EPC_Attack || AttackerCondition == EPlayerCondition::EPC_UpAttack || AttackerCondition == EPlayerCondition::EPC_DownAttack ||AttackerCondition == EPlayerCondition::EPC_UpperAttack || GetMovementComponent()->IsFalling()) {
		if(AttackerCondition == EPlayerCondition::EPC_Attack) return (AttackCnt == 4) ? AttackData->LastAk : AttackData->Ak;
		else if (AttackerCondition == EPlayerCondition::EPC_UpAttack || AttackerCondition == EPlayerCondition::EPC_DownAttack) return (AttackCnt == 5) ? AttackData->ExtensionLastAk : AttackData->ExtensionAk;
		else if (GetMovementComponent()->IsFalling()) return (AttackCnt == 6) ? AttackData->AirLastAk : AttackData->AirAk;
		else if (AttackerCondition == EPlayerCondition::EPC_UpperAttack) return AttackData->ExtensionLastAk;
	}
	else if (AttackerCondition == EPlayerCondition::EPC_Skill1)	return AttackData->SkillAk1;
	else if (AttackerCondition == EPlayerCondition::EPC_Skill2) return AttackData->SkillAk2;
	else if (AttackerCondition == EPlayerCondition::EPC_Grap) return AttackData->GrapAk;

	return 0;
}
void ANPlayer::DecreasedHealth(int8 DamageSize) {
	//GetHealthManager()->SetDecreaseHealth(DamageSize);
	GetHealthManager()->SetDecreaseHealth(40);
	GetCurAttackComp()->RotateToActor();

	UpdateWidget(EWidgetState::EWS_Health);
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
	PlayerCondition = NewCondition;
	ServerSetPlayerCondition(NewCondition);
}
void ANPlayer::ServerSetPlayerCondition_Implementation(EPlayerCondition NewCondition) {
	//SetPlayerCondition(NewCondition);
	//UE_LOG(LogTemp, Warning, TEXT("Changed Condition : %s"), *GetEnumToString(NewCondition));
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
	if(IsPlayerCondition(EPlayerCondition::EPC_Idle) || IsPlayerCondition(EPlayerCondition::EPC_Charge)) {
		SetPlayerCondition(EPlayerCondition::EPC_Charge);
	}
	else {
		EndChacra();
		return;
	}
	
	if (GetCurChacraComp()->GetChacra() >= 100.f) {
		bisCharing = true;
		EndChacra();
		return;
	}

	UE_LOG(LogTemp,Warning,TEXT("Is Working"));

	/** ChacraCharging in few Seconds */
	int64 HoldingTime = ChacraPressedTime / 10000;
	if (HoldingTime >= ChacraChargingSec) {
		GetCurChacraComp()->ChargingChacra();
		bisCharing = true;
	}
}
void ANPlayer::EndChacra() {
	if(!bisCharing) GetCurChacraComp()->UseChacra();
	else GetCurChacraComp()->ServerDestroyCurParticle();              //else ��ũ�� ��¡ ������ �Լ� ȣ��

	SetPlayerCondition(EPlayerCondition::EPC_Idle);
}
void ANPlayer::UpdateWidget_Implementation(const EWidgetState state) {
	if (MainPlayerController) {
		if (MainPlayerState) {
			switch (state)
			{
			case EWidgetState::EWS_Health:
				MainPlayerState->SetState(state, GetHealthManager()->GetCurrentHealth());
				break;
			case EWidgetState::EWS_Chacra:
				MainPlayerState->SetState(state, GetCurChacraComp()->GetChacra());
				break;
			case EWidgetState::EWS_Switch:
				MainPlayerState->SetState(state, SideStepCnt);
				break;
			case EWidgetState::EWS_Score:
				MainPlayerState->SetState(state, Score);
				break;
			default:
				break;
			}
		}

		// Update Widget
		GetMainController()->SetWidget(state);
		AnotherPlayer->GetMainController()->SetWidget(state);
	}
}
bool ANPlayer::UpdateWidget_Validate(const EWidgetState state) {
	return true;
}
void ANPlayer::ChacraDash() {
	if (AnotherPlayer) {
		UE_LOG(LogTemp, Warning, TEXT("Chacra Dash to %s"), *AnotherPlayer->GetName());
		
		SetPlayerCondition(EPlayerCondition::EPC_Dash);
		CurChacraComp->ResetChacraCnt(EChacraActions::ECA_Dash);
		CurAttackComp->ResetAll();

		// Animation
		GetMontageManager()->PlayNetworkMontage(GetMontageManager()->GetActionMontage().MT_ChacraDash, 0.f, GetPlayerCondition());

		// Reset Timer
		GetWorld()->GetTimerManager().ClearTimer(StopChacraDashHandle);
		GetWorld()->GetTimerManager().SetTimer(StopChacraDashHandle, this, &ANPlayer::StopChacraDash, 1.5f, false);
	}
}
void ANPlayer::AutoChacraDash(float DeltaTime) {
	if (IsPlayerCondition(EPlayerCondition::EPC_Dash)) {
		if (AP_Distance < ChacraDashStopDis) {
			StopChacraDash();
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
	GetWorld()->GetTimerManager().ClearTimer(StopChacraDashHandle);				//Ÿ�̸� �ʱ�ȭ
	SetPlayerCondition(EPlayerCondition::EPC_Idle);
	GetMontageManager()->StopNetworkMontage();

	// Set Player Condition if Crash 'in Air'
	if (GetMovementComponent()->IsFalling()) {
		SetAllGravity(0.f);

		AnotherPlayer->SetPlayerCondition(EPlayerCondition::EPC_Idle);
	}
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
		
		// Particle & Reset Chacra..
		if(SideStepParticle) {
			GetCurChacraComp()->ResetChacraCnt(EChacraActions::ECA_None,false);
			GetCurChacraComp()->ServerDestroyCurParticle();
			GetCurChacraComp()->SpawnCurParticle(SideStepParticle);
		}

		//Update Widget
		UpdateWidget(EWidgetState::EWS_Switch);

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

		//Update Widget
		UpdateWidget(EWidgetState::EWS_Switch);

		GetWorld()->GetTimerManager().ClearTimer(SideStepHandle);
		GetWorld()->GetTimerManager().SetTimer(SideStepHandle, this, &ANPlayer::RecoverSideStepCnt, 3.f, false);
	}
}
void ANPlayer::SetGravity(float val) {
	GravityVal = val;
	isGravityDone = false;

	ServerSetGravity(val);
}
void ANPlayer::MultiSetGravity_Implementation(float val) {
	GravityVal = val;
	isGravityDone = false;
}
bool ANPlayer::MultiSetGravity_Validate(float val) {
	return true;
}
void ANPlayer::ServerSetGravity_Implementation(float val) {
	MultiSetGravity(val);
}
bool ANPlayer::ServerSetGravity_Validate(float val) {
	return true;
}
void ANPlayer::SetAllGravity(float val) {
	GravityVal = val;
	AnotherPlayer->GravityVal = val;
	isGravityDone = false;
	AnotherPlayer->isGravityDone = false;

	ServerSetAllGravity();
}
void ANPlayer::MultiSetAllGravity_Implementation(float val) {
	GravityVal = val;
	AnotherPlayer->GravityVal = val;
	isGravityDone = false;
	AnotherPlayer->isGravityDone = false;

	if (GravityVal != 1.f) {
		FVector TmpVec = AnotherPlayer->GetActorLocation() + (AnotherPlayer->GetActorForwardVector() * 100);
		SetActorLocation(TmpVec);
	}
}
bool ANPlayer::MultiSetAllGravity_Validate(float val) {
	return true;
}
void ANPlayer::ServerSetAllGravity_Implementation(float val) {
	MultiSetAllGravity(val);
}
bool ANPlayer::ServerSetAllGravity_Validate(float val) {
	return true;
}
void ANPlayer::EndGravity() {
	GravityVal = 1.f;
	isGravityDone = false;
}
void ANPlayer::UpdateGravity() {
	//if (!isGravityDone && GravityVal != GetCharacterMovement()->GravityScale) {
	if (!isGravityDone) {
		GetCharacterMovement()->GravityScale = GravityVal;
		SetPlayerCondition(EPlayerCondition::EPC_Idle);
		isGravityDone = true;

		if (GravityVal != 1.f) {
			GetMovementComponent()->StopMovementImmediately();
			GetWorld()->GetTimerManager().ClearTimer(GravityHandle);
			GetWorld()->GetTimerManager().SetTimer(GravityHandle, this, &ANPlayer::EndGravity, 0.8f, false);
		}
	}
}
void ANPlayer::ThrowStar() {
	bool bIsChacraThrow = false;
	if (GetCurChacraComp()->GetChacraCnt() >= 1) {
		GetCurChacraComp()->ResetChacraCnt(EChacraActions::ECA_NinjaStar);
		bIsChacraThrow = true;
	}
	CurAttackComp->ThrowNinjaStar(bIsChacraThrow);
}
void ANPlayer::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANPlayer, CurrentWeapon);
	DOREPLIFETIME(ANPlayer, bIsDoubleJump);
	DOREPLIFETIME(ANPlayer, PlayerCondition);
}