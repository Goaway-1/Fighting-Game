#include "NWeapon.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

ANWeapon::ANWeapon(){
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	SetRootComponent(MeshComp);

	//SetReplicates(true);
	bReplicates = true;
}
void ANWeapon::PostInitProperties() {
	Super::PostInitProperties();

	if (WeaponMeshType.Num() < 2) UE_LOG(LogTemp, Warning, TEXT("Please Set WeaponMeshType of Weapon class"));
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
void ANWeapon::ServerSetWeaponRandom_Implementation(EWeaponType ChangeWeaponType){
	if(ChangeWeaponType == EWeaponType::EWT_Blade) MeshComp->SetStaticMesh(WeaponMeshType[1]);
	else MeshComp->SetStaticMesh(WeaponMeshType[0]);
}

void ANWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(ANWeapon, MeshComp);
	DOREPLIFETIME(ANWeapon, WeaponType);
}