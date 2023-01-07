#include "NWeapon.h"
#include "Components/StaticMeshComponent.h"
#include "NPlayer.h"
#include "MontageManager.h"
#include "AttackActorComponent.h"
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
	if (WeaponMeshType.Num() >= 2) {
		UE_LOG(LogTemp, Warning, TEXT("%s SetWeaponRandom is Called!"),*GetOwner()->GetName());
		int32 WeaponTmp = FMath::RandRange(0, 1);
		if (WeaponTmp == 0) {
			MeshComp->SetStaticMesh(WeaponMeshType[WeaponTmp]);
			WeaponType = EWeaponType::EWT_Sword;
		}
		else {
			MeshComp->SetStaticMesh(WeaponMeshType[WeaponTmp]);
			WeaponType = EWeaponType::EWT_Blade;
		}
	}

	/** Set AttackController */
	OwnPlayer = Cast<ANPlayer>(GetOwner());
	AttackController = OwnPlayer->GetCurAttackComp();
}
void ANWeapon::OnAttackBoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (HasAuthority() && OtherActor != this->GetOwner() && !AttackController->IsAlreadyOverlap(OtherActor)) {
		AttackController->SetOverlapActors(OtherActor);

		// 피격 몽타주 실행
		ANPlayer* victim = Cast<ANPlayer>(OtherActor);
		victim->IsHited();
	}
}
void ANWeapon::SetCollisionONOFF(bool isSet) {
	if (HasAuthority()) {
		if (!isSet) {
			MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			//UE_LOG(LogTemp, Warning, TEXT("Collision OFF"));
		}
		else {
			MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			AttackController->ClearOverlapActors();
			//UE_LOG(LogTemp, Warning, TEXT("Collision ON"));
		}
	}
}
void ANWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANWeapon, WeaponType);
}