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
	
#pragma region Default
public:	
	ANWeapon();

protected:
	virtual void PostInitProperties() override;
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	class ANPlayer* OwnPlayer;

	UPROPERTY(Replicated, VisibleAnywhere,Category = "Weapon")
	EWeaponType WeaponType;

	/* Please Set Weapons Mesh */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TArray<UStaticMesh*> WeaponMeshType;

	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Weapon")
	UStaticMeshComponent* MeshComp;

	/** If Overlap other Actor */
	UFUNCTION()
	void OnAttackBoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY()
	class UAttackActorComponent* AttackController;
public:	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE EWeaponType GetWeaponType() { return WeaponType; }

	UFUNCTION()
	int SetWeaponRandom();

	UFUNCTION(BlueprintCallable)
	void SetCollisionONOFF(bool isSet);

#pragma endregion
};