#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NWeapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8 {
	EWT_Sword			UMETA(DisplayName = "Sword"),
	EWT_Blade			UMETA(DisplayName = "Blade"),

	EWT_Default			UMETA(DisplayName = "Default")
};

UCLASS()
class NARUTO_API ANWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	ANWeapon();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	EWeaponType WeaponType;

	/* Please Set Weapons Mesh */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TArray<UStaticMesh*> WeaponMeshType;

	UPROPERTY(Replicated,EditDefaultsOnly,Category = "Weapon")
	UStaticMeshComponent* MeshComp;

	/* Set Player's Weapon for Random */
	//UFUNCTION()
	//void SetWeaponRandom();
	
	UFUNCTION(Server, Reliable)
	void ServerSetWeaponRandom(EWeaponType ChangeWeaponType);
public:	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE EWeaponType GetWeaponType() { return WeaponType; }

	UFUNCTION()
	void SetWeaponRandom();

};