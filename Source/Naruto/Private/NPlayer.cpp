#include "NPlayer.h"
#include "NPlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NCameraManager.h"
#include "NGameMode.h"
#include "NWeapon.h"
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
}
void ANPlayer::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	/** Setting the rotation of the controller according to the rotation of the CameraManager */
	if (CameraManager && HasAuthority()) {
		FRotator NRot = FRotator(CameraManager->GetActorRotation().Pitch, CameraManager->GetActorRotation().Yaw, GetController()->GetControlRotation().Roll);
		if (MainPlayerController) {
			MainPlayerController->ClientSetRotation(NRot);
		}
		else {
			MainPlayerController = Cast<ANPlayerController>(GetController());
		}
	}

	/** Get Another Player */
	SetAnotherPlayer();

	/** Active Chacra Dash */
	AutoChacraDash(DeltaTime);

	// Test : ON&OFF Gravitiy
	if (!bIsGravityHandling && (IsPlayerCondition(EPlayerCondition::EPC_AirAttack) || IsPlayerCondition(EPlayerCondition::EPC_AirHited))) {
		SetGravity(0.f);

		bIsGravityHandling = true;
		FTimerDelegate TimerDel;
		TimerDel.BindUFunction(this, FName("SetGravity"), 1.f);
		GetWorld()->GetTimerManager().ClearTimer(GravityHandle);
		GetWorld()->GetTimerManager().SetTimer(GravityHandle, TimerDel, ResetGravityTime, false);
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
	PlayerInputComponent->BindAction("Chacra", IE_Pressed,this, &ANPlayer::Chacra);
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
	if(IsPlayerCondition(EPlayerCondition::EPC_CantMove) || IsPlayerCondition(EPlayerCondition::EPC_Attack) || IsPlayerCondition(EPlayerCondition::EPC_Dash) || IsPlayerCondition(EPlayerCondition::EPC_Hited)) return false;
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
	if (IsPlayerCondition(EPlayerCondition::EPC_CantMove) || IsPlayerCondition(EPlayerCondition::EPC_Dash) || IsPlayerCondition(EPlayerCondition::EPC_Hited)) return;

	CurAttackComp->DefaultAttack_KeyDown(GetKeyUpDown());
}
void ANPlayer::IsHited() {
	SetPlayerCondition(EPlayerCondition::EPC_Hited);
	/** Check Attack Information... (Normal or Skill) */
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
	//else if(공중인 경우....)
	else  {
		UE_LOG(LogTemp, Warning, TEXT("%s attack %s"), *AnotherPlayer->GetName(), *this->GetName());

		/** 공중으로 날려! */
		if (AnotherPlayer->IsPlayerCondition(EPlayerCondition::EPC_UpperAttack)) {
			UE_LOG(LogTemp, Warning, TEXT("Upper Vitcim!"));
			SetPlayerCondition(EPlayerCondition::EPC_UpperHited);

			// Launch
			FVector ForceVec = (GetActorForwardVector() * -500.f) + FVector(0.f, 0.f, 2000.f);
			LaunchCharacter(ForceVec, false, false);

			// ANim
			UAnimMontage* mon = AnotherPlayer->GetMontageManager()->GetActionMontage().MT_JumpVictim;
			GetMontageManager()->PlayNetworkMontage(mon, 1.f, GetPlayerCondition(), AnotherPlayer->GetCurAttackComp()->GetComboCnt());
			
			// 테스트..
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
void ANPlayer::SetPlayerCondition(EPlayerCondition NewCondition) {
	PlayerCondition = NewCondition;
	if(IsLocallyControlled()) ServerSetPlayerCondition(NewCondition);
}
void ANPlayer::MultiSetPlayerCondition_Implementation(EPlayerCondition NewCondition) {
	PlayerCondition = NewCondition;
}
bool ANPlayer::MultiSetPlayerCondition_Validate(EPlayerCondition NewCondition) {
	return true;
}
void ANPlayer::ServerSetPlayerCondition_Implementation(EPlayerCondition NewCondition) {
	MultiSetPlayerCondition(NewCondition);
}
bool ANPlayer::ServerSetPlayerCondition_Validate(EPlayerCondition NewCondition) {
	return true;
}
void ANPlayer::Chacra() {
	if (!IsCanMove()) return;

	CurChacraComp->UseChacra();
}
void ANPlayer::ChacraDash() {
	if (AnotherPlayer) {
		UE_LOG(LogTemp, Warning, TEXT("Chacra Dash to %s"), *AnotherPlayer->GetName());

		if (!HasAuthority()) ServerChacraDash();

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
			GetMovementComponent()->StopMovementImmediately();
			AnotherPlayer->GetMovementComponent()->StopMovementImmediately();
			
			// 테스트중.. :  상태 변경하여서
			SetPlayerCondition(EPlayerCondition::EPC_AirAttack);
			AnotherPlayer->SetPlayerCondition(EPlayerCondition::EPC_AirHited);
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
	if (AP_Distance < ChacraDashStopDis || IsPlayerCondition(EPlayerCondition::EPC_Dash)) {
		SetPlayerCondition(EPlayerCondition::EPC_Idle);
		GetMontageManager()->StopMontage();
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
void ANPlayer::SetGravity(float val) {
	GetCharacterMovement()->GravityScale = val;
	SetPlayerCondition(EPlayerCondition::EPC_Idle);
}
void ANPlayer::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANPlayer, CurrentWeapon);
	DOREPLIFETIME(ANPlayer, bIsDoubleJump);
	DOREPLIFETIME(ANPlayer, PlayerCondition);
	DOREPLIFETIME(ANPlayer, SideStepCnt);
	DOREPLIFETIME(ANPlayer, AnotherPlayer);
}