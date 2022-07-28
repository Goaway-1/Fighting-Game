#include "NWeapon.h"
#include "Components/StaticMeshComponent.h"

ANWeapon::ANWeapon(){
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	SetRootComponent(MeshComp);
	if(WeaponMeshType.Num() >= 2) MeshComp->SetStaticMesh(WeaponMeshType[0]);

	SetReplicates(true);
}
void ANWeapon::BeginPlay(){
	Super::BeginPlay();

	SetWeaponRandom();
}
void ANWeapon::SetWeaponRandom() {
	if (WeaponMeshType.Num() >= 2) {
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
}