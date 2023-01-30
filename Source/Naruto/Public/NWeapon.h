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
	virtual void PostInitProperties() override;
	virtual void BeginPlay() override;

	TWeakObjectPtr<class ANPlayer>  OwnPlayer;
	class UAttackManager* AttackController;

	UPROPERTY(Replicated)
	EWeaponType WeaponType;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TArray<UStaticMesh*> WeaponMeshType;				// Set Weapons Mesh

	UPROPERTY(Replicated)
	UStaticMeshComponent* MeshComp;

	/** If Overlap other Actor */
	UFUNCTION()
	void OnAttackBoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
public:	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE EWeaponType GetWeaponType() { return WeaponType; }

	UFUNCTION(BlueprintCallable)
	void SetCollisionONOFF(bool isSet);

	void SetWeaponRandom();
};