#include "AttackActorComponent.h"
#include "ChacraActorComponent.h"
#include "NPlayerState.h"
#include "NPlayer.h"
#include "NinjaStar.h"
#include "NPlayerController.h"
#include "MontageManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

UAttackActorComponent::UAttackActorComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);
}
void UAttackActorComponent::BeginPlay() {
	Super::BeginPlay();
}
void UAttackActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Air Attack...
	bool isFalling = Cast<ANPlayer>(GetOwner())->GetMovementComponent()->IsFalling();
	if (!isFalling) {
		bAirAttackEnd = false;	
		bCanAirAttack = true;
	}
}
void UAttackActorComponent::SetOverlapActors(AActor* actor) {
	OverlapActors.Add(actor);
}
TArray<AActor*> UAttackActorComponent::GetOverlapActors() {
	return OverlapActors;
}
bool UAttackActorComponent::IsAlreadyOverlap(AActor* actor) { 
	for (int i = 0; i < OverlapActors.Num();i++) { 
		if (OverlapActors[i] == actor) return true;
	} 
	return false;
}
void UAttackActorComponent::ClearOverlapActors() { 
	OverlapActors.Reset(); 
}
void UAttackActorComponent::DefaultAttack_KeyDown(EKeyUpDown KeyUD) {
	TmpKeyUD = KeyUD;

	if (!bAttacking) Attack();
	else bIsAttackCheck = true;
}
void UAttackActorComponent::Attack() {
	if(bAirAttackEnd) return;

	bAttacking = true;

	/** Rotate to another Actor.. (Network & MutiCast) 	*/
	RotateToActor();

	/** 상태에 따른 액션.. */
	if(CurOwner->GetMontageManager()){
		UChacraActorComponent* ChacraCom = CurOwner->GetCurChacraComp();
		bool isFalling = Cast<ANPlayer>(GetOwner())->GetMovementComponent()->IsFalling();

		if (isFalling && !bAirAttackEnd){/** 공중 공격 */
			CurOwner->SetPlayerCondition(EPlayerCondition::EPC_AirAttack);
			if (!CurOwner->GetMontageManager()->IsMontagePlaying(CurOwner->GetMontageManager()->GetActionMontage().MT_JumpAttack)) {
				if (!bCanAirAttack) return;

				SetComoboCnt(1);
				ClearOverlapActors();
			}

			CurOwner->GetMontageManager()->PlayNetworkMontage(CurOwner->GetMontageManager()->GetActionMontage().MT_JumpAttack, 1.f, CurOwner->GetPlayerCondition(), ComboCnt);
			
			CurOwner->SetGravity(0.f);
		}
		else if (CurOwner->GetPlayerCondition() == EPlayerCondition::EPC_Block) {
			UE_LOG(LogTemp, Warning, TEXT("Grap Attack"));
			CurOwner->SetPlayerCondition(EPlayerCondition::EPC_Grap);
			CurOwner->GetMontageManager()->PlayNetworkMontage(CurOwner->GetMontageManager()->GetActionMontage().MT_GrapAttack, 1.f,CurOwner->GetPlayerCondition());
		}
		else if (ChacraCom->GetChacraCnt() > 0) {
			if(ChacraCom->GetChacraCnt() == 1) {
				UE_LOG(LogTemp, Warning, TEXT("Chacra1 Attack"));
				CurOwner->SetPlayerCondition(EPlayerCondition::EPC_Skill1);
				CurOwner->GetMontageManager()->PlayNetworkMontage(CurOwner->GetMontageManager()->GetActionMontage().ChacraAttack.MTChacra_Attacker[0], 1.f, CurOwner->GetPlayerCondition());
				ChacraCom->ResetChacraCnt();
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("Chacra2 Attack"));
				CurOwner->SetPlayerCondition(EPlayerCondition::EPC_Skill2);
				CurOwner->GetMontageManager()->PlayNetworkMontage(CurOwner->GetMontageManager()->GetActionMontage().ChacraAttack.MTChacra_Attacker[1], 1.f, CurOwner->GetPlayerCondition());
				ChacraCom->ResetChacraCnt();
			}
		}
		else {	
			CurOwner->SetPlayerCondition(EPlayerCondition::EPC_Attack); 
			if (!CurOwner->GetMontageManager()->IsMontagePlaying(CurOwner->GetMontageManager()->GetActionMontage().MT_Attacker) &&
				!CurOwner->GetMontageManager()->IsMontagePlaying(CurOwner->GetMontageManager()->GetActionMontage().AttackSplit.MTUP_Attacker) && 
				!CurOwner->GetMontageManager()->IsMontagePlaying(CurOwner->GetMontageManager()->GetActionMontage().AttackSplit.MTDOWN_Attacker)) {	// First Attack...
				ComboCnt = 1;
				SetComoboCnt(ComboCnt);
				CurOwner->GetMontageManager()->PlayNetworkMontage(CurOwner->GetMontageManager()->GetActionMontage().MT_Attacker, 1.f, CurOwner->GetPlayerCondition(), 1);
			}
			else {
				if (CurKeyUD == EKeyUpDown::EKUD_Up) {
					CurOwner->GetMontageManager()->PlayNetworkMontage(CurOwner->GetMontageManager()->GetActionMontage().AttackSplit.MTUP_Attacker, 1.f, CurOwner->GetPlayerCondition(), ComboCnt);
					if (ComboCnt == 5) {
						UE_LOG(LogTemp, Warning, TEXT("Upper Attack!"));
						CurOwner->SetPlayerCondition(EPlayerCondition::EPC_UpperAttack);
					}
				}
				else if (CurKeyUD == EKeyUpDown::EKUD_Down) CurOwner->GetMontageManager()->PlayNetworkMontage(CurOwner->GetMontageManager()->GetActionMontage().AttackSplit.MTDOWN_Attacker, 1.f, CurOwner->GetPlayerCondition(), ComboCnt);
				else CurOwner->GetMontageManager()->PlayNetworkMontage(CurOwner->GetMontageManager()->GetActionMontage().MT_Attacker, 1.f, CurOwner->GetPlayerCondition(), ComboCnt);
			}
		}
	}
}
void UAttackActorComponent::EndAttack() {
	if (ComboCnt == 6) CurOwner->SetGravity(1.f);

	CurOwner->SetPlayerCondition(EPlayerCondition::EPC_Idle); 
	bAttacking = false;
	CurKeyUD = EKeyUpDown::EKUD_Default;
	SetComoboCnt(1);
	ClearOverlapActors();
}
void UAttackActorComponent::AttackInputCheck() {	
	bool isFalling = Cast<ANPlayer>(GetOwner())->GetMovementComponent()->IsFalling();
	if (isFalling) {
		bCanAirAttack = false;
		if (OverlapActors.Num() <= 0) {		/** Last Attack & Not Hit then fall Down.. */
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
	else if (bIsAttackCheck) {
		SetComoboCnt(ComboCnt + 1);
		if (CurOwner->GetMontageManager()->GetActionMontage().splitIdx == ComboCnt) CurKeyUD = TmpKeyUD;
		bIsAttackCheck = false;
		Attack();
	}
	ClearOverlapActors();
}
void UAttackActorComponent::GrapHitedCheck() {
	if (bGrapHited) {
		UE_LOG(LogTemp, Warning, TEXT("Grap Attack Hited!"));
		CurOwner->GetMontageManager()->PlayNetworkMontage(CurOwner->GetMontageManager()->GetActionMontage().MT_GrapAttack, 1.f, CurOwner->GetPlayerCondition(), 1);
		bGrapHited = false;
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Grap Attack Not Hited!"));
	}
}
void UAttackActorComponent::RotateToActor() {
	/** Rotate (Fixed Roll & Pitch) */

	if (CurOwner && CurOwner->IsAnotherPlayer()) {
		FRotator RotateVal = UKismetMathLibrary::FindLookAtRotation(CurOwner->GetActorLocation(), CurOwner->GetAnotherLocation());
		RotateVal.Roll = CurOwner->GetActorRotation().Roll;
		RotateVal.Pitch = CurOwner->GetActorRotation().Pitch;
		CurOwner->SetActorRotation(RotateVal);
		ServerRotateToActor(RotateVal);
	}
	else UE_LOG(LogTemp, Warning, TEXT("[AttackActorCopoment_ERROR] : Roate Attack is Not Working"));
}
void UAttackActorComponent::MultiRotateToActor_Implementation(FRotator Rot){
	//if (!Cast<ACharacter>(GetOwner())->IsLocallyControlled()) {
		GetOwner()->SetActorRotation(Rot);		
	//}
}
bool UAttackActorComponent::MultiRotateToActor_Validate(FRotator Rot) {
	return true;
}
void UAttackActorComponent::ServerRotateToActor_Implementation(FRotator Rot) {
	MultiRotateToActor(Rot);
}
bool UAttackActorComponent::ServerRotateToActor_Validate(FRotator Rot) {
	return true;
}
void UAttackActorComponent::SetComoboCnt(int16 cnt){
	ComboCnt = cnt;
	ServerSetComboCnt(cnt);
}
void UAttackActorComponent::MultiSetComoboCnt_Implementation(int16 cnt) {
	ComboCnt = cnt;
}
bool UAttackActorComponent::MultiSetComoboCnt_Validate(int16 cnt) {
	return true;
}
void UAttackActorComponent::ServerSetComboCnt_Implementation(int16 cnt) {
	ComboCnt = cnt;
}
bool UAttackActorComponent::ServerSetComboCnt_Validate(int16 cnt) {
	return true;
}
void UAttackActorComponent::ResetAll() {
	SetComoboCnt(1);
	bAttacking = false;
	bIsAttackCheck = false;
	bAirAttackEnd = false;
	bCanAirAttack = true;
	ClearOverlapActors();
}
void UAttackActorComponent::ThrowNinjaStar(bool bIsChacra) {
	if (GetOwner()->GetLocalRole() != ROLE_Authority) ServerThrowNinjaStar(bIsChacra);
	else if (!NinjaStar && NinjaStarClass) {
		RotateToActor();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = GetOwner()->GetInstigator();

		/** Set INIT (Loc & Rot) */
		FVector SpawnVec = CurOwner->GetActorLocation() + (CurOwner->GetActorForwardVector() * 100.f);
		FRotator Rot = UKismetMathLibrary::FindLookAtRotation(SpawnVec, CurOwner->GetAnotherLocation());
		NinjaStar = GetWorld()->SpawnActor<ANinjaStar>(NinjaStarClass, SpawnVec, Rot, SpawnParams);
		NinjaStar->InitSetting(CurOwner->GetAnotherPlayer(), bIsChacra);

		/** Set Timer */
		GetWorld()->GetTimerManager().ClearTimer(NinjaStarHandle);
		GetWorld()->GetTimerManager().SetTimer(NinjaStarHandle, this, &UAttackActorComponent::ResetNinjaStar, 2.f, false);
	}
}
void UAttackActorComponent::ServerThrowNinjaStar_Implementation(bool bIsChacra) {
	ThrowNinjaStar(bIsChacra);
}
bool UAttackActorComponent::ServerThrowNinjaStar_Validate(bool bIsChacra) {
	return true;
}
void UAttackActorComponent::ResetNinjaStar() {
	UE_LOG(LogTemp, Warning, TEXT("NinjaStar is Reset"));
	NinjaStar = nullptr;
}
void UAttackActorComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAttackActorComponent, OverlapActors);
	DOREPLIFETIME(UAttackActorComponent, InRangeActor);
	DOREPLIFETIME(UAttackActorComponent, ComboCnt);
	DOREPLIFETIME(UAttackActorComponent, CurKeyUD);
	DOREPLIFETIME(UAttackActorComponent, TmpKeyUD);
}