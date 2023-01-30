#include "NWeapon.h"
#include "Components/StaticMeshComponent.h"
#include "NPlayer.h"
#include "MontageManager.h"
#include "AttackManager.h"
#include "Net/UnrealNetwork.h"

ANWeapon::ANWeapon(){
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	SetRootComponent(MeshComp);
	MeshComp->SetIsReplicated(true);
	MeshComp->OnComponentBeginOverlap.AddDynamic(this, &ANWeapon::OnAttackBoxOverlapBegin);

	bReplicates = true;
}
void ANWeapon::PostInitProperties() {
	Super::PostInitProperties();

	if (WeaponMeshType.Num() < 2) UE_LOG(LogTemp, Warning, TEXT("Please Set WeaponMeshType of Weapon class"));
}
void ANWeapon::BeginPlay(){
	Super::BeginPlay();

	SetCollisionONOFF(false);
}
void ANWeapon::SetWeaponRandom() {
	/** Set AttackController */
	OwnPlayer = Cast<ANPlayer>(GetOwner());
	AttackController = OwnPlayer->GetAttackManager();

	if (WeaponMeshType.Num() >= 2) {
		int WeaponTmp = FMath::RandRange(0, 1);
		MeshComp->SetStaticMesh(WeaponMeshType[WeaponTmp]);
		WeaponType = (WeaponTmp == 0) ? EWeaponType::EWT_Sword : EWeaponType::EWT_Blade;
	}
}
void ANWeapon::OnAttackBoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (HasAuthority() && OtherActor != this->GetOwner() && !AttackController->IsAlreadyOverlap(OtherActor)) {
		AttackController->SetOverlapActors(OtherActor);

		ANPlayer* victim = Cast<ANPlayer>(OtherActor);
		victim->IsHited(OwnPlayer->GetPlayerCondition(), OwnPlayer->GetAttackManager()->GetComboCnt());
	}
}
void ANWeapon::SetCollisionONOFF(bool isSet) {
	if (HasAuthority()) {
		if (!isSet) MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		else MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}
void ANWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANWeapon, WeaponType);
	DOREPLIFETIME(ANWeapon, MeshComp);	
}