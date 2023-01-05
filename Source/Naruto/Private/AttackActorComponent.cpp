#include "AttackActorComponent.h"
#include "ChacraActorComponent.h"
#include "NPlayerState.h"
#include "NPlayer.h"
#include "NPlayerController.h"
#include "MontageManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

UAttackActorComponent::UAttackActorComponent() {
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true);
}
void UAttackActorComponent::BeginPlay() {
	Super::BeginPlay();
}
void UAttackActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

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
	bAttacking = true;

	/** Rotate to another Actor.. (Network & MutiCast) 	*/
	RotateToActor();

	/** Play Animation Montage */
	if(CurOwner->GetMontageManager()){
		UChacraActorComponent* ChacraCom = CurOwner->GetCurChacraComp();
		bool isFalling = Cast<ANPlayer>(GetOwner())->GetMovementComponent()->IsFalling();

		if (isFalling){	
			CurOwner->SetPlayerCondition(EPlayerCondition::EPC_Attack); 

			EndAttack(); // 임시 (삭제)
		}
		else if (CurOwner->IsPlayerCondition(EPlayerCondition::EPC_Block)) {
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
			if (!CurOwner->GetMontageManager()->IsMontagePlaying(CurOwner->GetMontageManager()->GetActionMontage().MT_Attacker) && !CurOwner->GetMontageManager()->IsMontagePlaying(CurOwner->GetMontageManager()->GetActionMontage().AttackSplit.MTUP_Attacker)) {	//공격중이 아닐때 (처음 공격)
				ComboCnt = 1;
				SetComoboCnt(ComboCnt);
				CurOwner->GetMontageManager()->PlayNetworkMontage(CurOwner->GetMontageManager()->GetActionMontage().MT_Attacker, 1.f, CurOwner->GetPlayerCondition(), 1);
			}
			else {
				if (CurKeyUD == EKeyUpDown::EKUD_Up) CurOwner->GetMontageManager()->PlayNetworkMontage(CurOwner->GetMontageManager()->GetActionMontage().AttackSplit.MTUP_Attacker, 1.f, CurOwner->GetPlayerCondition(), ComboCnt);
				else if (CurKeyUD == EKeyUpDown::EKUD_Down) CurOwner->GetMontageManager()->PlayNetworkMontage(CurOwner->GetMontageManager()->GetActionMontage().AttackSplit.MTDOWN_Attacker, 1.f, CurOwner->GetPlayerCondition(), ComboCnt);
				else CurOwner->GetMontageManager()->PlayNetworkMontage(CurOwner->GetMontageManager()->GetActionMontage().MT_Attacker, 1.f, CurOwner->GetPlayerCondition(), ComboCnt);
			}
		}
	}
}
void UAttackActorComponent::EndAttack() {
	CurOwner->SetPlayerCondition(EPlayerCondition::EPC_Idle); 
	bAttacking = false;
	CurKeyUD = EKeyUpDown::EKUD_Default;
	ComboCnt = 1;
	SetComoboCnt(ComboCnt);
}
void UAttackActorComponent::AttackInputCheck() {
	if (bIsAttackCheck) {
		ComboCnt++;
		SetComoboCnt(ComboCnt);
		if (CurOwner->GetMontageManager()->GetActionMontage().splitIdx == ComboCnt) CurKeyUD = TmpKeyUD;
		bIsAttackCheck = false;
		Attack();
	}
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

	if (CurOwner && CurOwner->GetAnotherPlayer()) {
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
	MultiSetComoboCnt(cnt);
}
bool UAttackActorComponent::ServerSetComboCnt_Validate(int16 cnt) {
	return true;
}
void UAttackActorComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAttackActorComponent, OverlapActors);
	DOREPLIFETIME(UAttackActorComponent, InRangeActor);
	DOREPLIFETIME(UAttackActorComponent, ComboCnt);
}