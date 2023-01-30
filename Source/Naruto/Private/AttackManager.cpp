#include "AttackManager.h"
#include "ChacraManager.h"
#include "NPlayerState.h"
#include "NPlayer.h"
#include "NinjaStar.h"
#include "NPlayerController.h"
#include "MontageManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"

UAttackManager::UAttackManager() {
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicated(true);
}
void UAttackManager::BeginPlay() {
	Super::BeginPlay();

	CurOwner = Cast<ANPlayer>(GetOwner());
}
void UAttackManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Check able to reset Air Attck 
	if (!bCanAirAttack) ResetAirAttack();
}
void UAttackManager::SetOverlapActors(AActor* actor) {
	OverlapActors.Add(actor);
}
TArray<AActor*> UAttackManager::GetOverlapActors() {
	return OverlapActors;
}
bool UAttackManager::IsAlreadyOverlap(AActor* actor) {
	for (int i = 0; i < OverlapActors.Num();i++) { 
		if (OverlapActors[i] == actor) return true;
	} 
	return false;
}
void UAttackManager::ClearOverlapActors() {
	OverlapActors.Reset(); 
}
void UAttackManager::DefaultAttack_KeyDown(EKeyUpDown KeyUD) {
	TmpKeyUD = KeyUD;

	if (!bAttacking) Attack();
	else bIsAttackCheck = true;
}
void UAttackManager::Attack() {
	if(bAirAttackEnd || !CurOwner.IsValid()) return;

	bAttacking = true;
	RotateToActor();

	/** Start Attack by Type */
	if(CurOwner->GetMontageManager()) {
		UChacraManager* ChacraCom = CurOwner->GetChacraManager();
		bool isFalling = Cast<ANPlayer>(GetOwner())->GetMovementComponent()->IsFalling();

		if (isFalling && !bAirAttackEnd){														// Air Attack
			CurOwner->SetPlayerCondition(EPlayerCondition::EPC_AirAttack);
			if (!CurOwner->GetMontageManager()->IsMontagePlaying(CurOwner->GetMontageManager()->GetActionMontage().MT_JumpAttack)) {
				if (!bCanAirAttack) return;

				SetComoboCnt(1);
				ClearOverlapActors();
			}

			CurOwner->GetMontageManager()->PlayNetworkMontage(CurOwner->GetMontageManager()->GetActionMontage().MT_JumpAttack, 1.f, CurOwner->GetPlayerCondition(), ComboCnt);
			CurOwner->SetGravity(0.f);
		}
		else if (CurOwner->GetPlayerCondition() == EPlayerCondition::EPC_Block) {				// Grap Attack
			UE_LOG(LogTemp, Warning, TEXT("Grap Attack"));
			CurOwner->SetPlayerCondition(EPlayerCondition::EPC_Grap);
			CurOwner->GetMontageManager()->PlayNetworkMontage(CurOwner->GetMontageManager()->GetActionMontage().MT_GrapAttack, 1.f,CurOwner->GetPlayerCondition());
		}
		else if (ChacraCom->GetChacraCnt() > 0) {												// Skill Attack
			if(ChacraCom->GetChacraCnt() == 1) {
				UE_LOG(LogTemp, Warning, TEXT("Chacra1 Attack"));
				CurOwner->SetPlayerCondition(EPlayerCondition::EPC_Skill1);
				CurOwner->GetMontageManager()->PlayNetworkMontage(CurOwner->GetMontageManager()->GetActionMontage().ChacraAttack.MTChacra_Attacker[0], 0.7f, CurOwner->GetPlayerCondition());
				ChacraCom->ResetChacraCnt(EChacraActions::ECA_Skill1);
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("Chacra2 Attack"));
				CurOwner->SetPlayerCondition(EPlayerCondition::EPC_Skill2);
				CurOwner->GetMontageManager()->PlayNetworkMontage(CurOwner->GetMontageManager()->GetActionMontage().ChacraAttack.MTChacra_Attacker[1], 0.7f, CurOwner->GetPlayerCondition());
				ChacraCom->ResetChacraCnt(EChacraActions::ECA_Skill2);
			}
		}
		else {																					// Default Attack
			CurOwner->SetPlayerCondition(EPlayerCondition::EPC_Attack); 

			/** Check First Attack or not */
			if (!CurOwner->GetMontageManager()->IsMontagePlaying(CurOwner->GetMontageManager()->GetActionMontage().MT_Attacker) &&
				!CurOwner->GetMontageManager()->IsMontagePlaying(CurOwner->GetMontageManager()->GetActionMontage().AttackSplit.MTUP_Attacker) && 
				!CurOwner->GetMontageManager()->IsMontagePlaying(CurOwner->GetMontageManager()->GetActionMontage().AttackSplit.MTDOWN_Attacker)) {	
				ComboCnt = 1;
				SetComoboCnt(ComboCnt);
				CurOwner->GetMontageManager()->PlayNetworkMontage(CurOwner->GetMontageManager()->GetActionMontage().MT_Attacker, 1.f, CurOwner->GetPlayerCondition(), 1);
			}
			else {
				/** Check Extension of an Attack */
				if (CurKeyUD == EKeyUpDown::EKUD_Up) {
					CurOwner->GetMontageManager()->PlayNetworkMontage(CurOwner->GetMontageManager()->GetActionMontage().AttackSplit.MTUP_Attacker, 1.f, CurOwner->GetPlayerCondition(), ComboCnt);
					if (ComboCnt == 5) CurOwner->SetPlayerCondition(EPlayerCondition::EPC_UpperAttack);
					else CurOwner->SetPlayerCondition(EPlayerCondition::EPC_UpAttack);
				}
				else if (CurKeyUD == EKeyUpDown::EKUD_Down) {
					CurOwner->GetMontageManager()->PlayNetworkMontage(CurOwner->GetMontageManager()->GetActionMontage().AttackSplit.MTDOWN_Attacker, 1.f, CurOwner->GetPlayerCondition(), ComboCnt);
					CurOwner->SetPlayerCondition(EPlayerCondition::EPC_DownAttack);
				}
				else CurOwner->GetMontageManager()->PlayNetworkMontage(CurOwner->GetMontageManager()->GetActionMontage().MT_Attacker, 1.f, CurOwner->GetPlayerCondition(), ComboCnt);
			}
		}
	}
}
void UAttackManager::EndAttack() {
	if (ComboCnt == 6) CurOwner->SetGravity(1.f);

	ResetAll();
}
void UAttackManager::AttackInputCheck() {
	bool isFalling = Cast<ANPlayer>(GetOwner())->GetMovementComponent()->IsFalling();
	if (isFalling) {
		bCanAirAttack = false;
		if (OverlapActors.Num() <= 0 && ComboCnt >= 2) {		/** Last Attack & Not Hit then fall Down.. */		
			bAirAttackEnd = true;
			bIsAttackCheck = false;
			bAttacking = false;
			CurOwner->SetGravity(1.f);
			EndAttack(); 
		}
	    else if (bIsAttackCheck) {								/** Continue Fly Attack.. */
			SetComoboCnt(ComboCnt + 1);
			bIsAttackCheck = false;
			Attack();
		}
	}
	else if (bIsAttackCheck) {									/** Continue Attack.. */
		SetComoboCnt(ComboCnt + 1);
		if (CurOwner->GetMontageManager()->GetActionMontage().splitIdx == ComboCnt) CurKeyUD = TmpKeyUD;
		bIsAttackCheck = false;
		Attack();
	}
	ClearOverlapActors();
}
void UAttackManager::GrapHitedCheck() {
	if (bGrapHited) {
		CurOwner->GetMontageManager()->PlayNetworkMontage(CurOwner->GetMontageManager()->GetActionMontage().MT_GrapAttack, 1.f, CurOwner->GetPlayerCondition(), 1);
		bGrapHited = false;
	}
}
void UAttackManager::ResetAirAttack() {
	if (CurOwner.IsValid() && !CurOwner->GetMovementComponent()->IsFalling()) {
		bAirAttackEnd = false;
		bCanAirAttack = true;
	}
}
void UAttackManager::RotateToActor() {
	/** Rotate (Fixed Roll & Pitch) */
	if (CurOwner.IsValid() && CurOwner->IsAnotherPlayer()) {
		FRotator RotateVal = UKismetMathLibrary::FindLookAtRotation(CurOwner->GetActorLocation(), CurOwner->GetAnotherLocation());
		RotateVal.Roll = CurOwner->GetActorRotation().Roll;
		RotateVal.Pitch = CurOwner->GetActorRotation().Pitch;
		CurOwner->SetActorRotation(RotateVal);
		ServerRotateToActor(RotateVal);
	}
	else UE_LOG(LogTemp, Warning, TEXT("[AttackActorCopoment_ERROR] : Roate Attack is Not Working"));
}
void UAttackManager::ServerRotateToActor_Implementation(FRotator Rot) {
	GetOwner()->SetActorRotation(Rot);
}
bool UAttackManager::ServerRotateToActor_Validate(FRotator Rot) {
	return true;
}
void UAttackManager::SetComoboCnt(int16 cnt){
	ComboCnt = cnt;
	ServerSetComboCnt(cnt);
}
void UAttackManager::ServerSetComboCnt_Implementation(int16 cnt) {
	ComboCnt = cnt;
}
bool UAttackManager::ServerSetComboCnt_Validate(int16 cnt) {
	return true;
}
void UAttackManager::ResetAll() {
	SetComoboCnt(1);
	bAttacking = false;
	bIsAttackCheck = false;
	bAirAttackEnd = false;
	bCanAirAttack = true;
	ClearOverlapActors();
	CurOwner->SetPlayerCondition(EPlayerCondition::EPC_Idle);
	CurKeyUD = EKeyUpDown::EKUD_Default;
}
void UAttackManager::ThrowNinjaStar(bool bIsChacra) {
	if (GetOwner()->GetLocalRole() != ROLE_Authority) ServerThrowNinjaStar(bIsChacra);
	else if (!NinjaStar && NinjaStarClass) {
		RotateToActor();

		/** Spawn NinjaStar */
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = GetOwner()->GetInstigator();
		FVector SpawnVec = CurOwner->GetActorLocation() + (CurOwner->GetActorForwardVector() * 100.f);
		FRotator Rot = UKismetMathLibrary::FindLookAtRotation(SpawnVec, CurOwner->GetAnotherLocation());
		NinjaStar = GetWorld()->SpawnActor<ANinjaStar>(NinjaStarClass, SpawnVec, Rot, SpawnParams);
		NinjaStar->InitSetting(CurOwner->GetAnotherPlayer(), bIsChacra);

		/** Play Sound */
		if(NinjaStar_Sound) UGameplayStatics::PlaySound2D(this, NinjaStar_Sound);

		/** Reset Timer */
		GetWorld()->GetTimerManager().ClearTimer(NinjaStarHandle);
		GetWorld()->GetTimerManager().SetTimer(NinjaStarHandle, this, &UAttackManager::ResetNinjaStar, 2.f, false);
	}
}
void UAttackManager::ServerThrowNinjaStar_Implementation(bool bIsChacra) {
	ThrowNinjaStar(bIsChacra);
}
bool UAttackManager::ServerThrowNinjaStar_Validate(bool bIsChacra) {
	return true;
}
void UAttackManager::ResetNinjaStar() {
	NinjaStar = nullptr;
}
void UAttackManager::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UAttackManager, OverlapActors, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UAttackManager, ComboCnt, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UAttackManager, CurKeyUD, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UAttackManager, TmpKeyUD, COND_OwnerOnly);
}