#pragma once

#include "CoreMinimal.h"
#include "AttackStruct.h"
#include "Components/ActorComponent.h"
#include "ChacraManager.generated.h"

/** Specifying Chacra Consumption */
UENUM(BlueprintType)
enum class EChacraActions : uint8 {
	ECA_None			UMETA(DisplayName = "None"),
	ECA_Dash			UMETA(DisplayName = "Dash"),
	ECA_NinjaStar		UMETA(DisplayName = "NinjaStar"),
	ECA_Skill1			UMETA(DisplayName = "Skill1"),
	ECA_Skill2			UMETA(DisplayName = "Skill2")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NARUTO_API UChacraManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UChacraManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma region CHACRA
protected:
	UPROPERTY(Replicated, VisibleAnywhere)
	int8 ChacraCnt;								// Active Count of Chacra..

	UPROPERTY()
	FTimerHandle ResetChacraHandle;

	TWeakObjectPtr<class ANPlayer> OwnPlayer;
private:
	const float MaxChacra = 100.f;
	const float ChargingVal = 0.7f;				// Increase Val

	UPROPERTY(Replicated, VisibleAnywhere, Category = "Chacra")
	float CurrentChacra = 100.f;

	/** Consumption by type */
	const float NormalChacraConsumption = 8.f;
	const float SkillChacraConsumption = 15.f;

public:
	FORCEINLINE float GetChacra() { return CurrentChacra; }

	UFUNCTION(Server, Reliable, WithValidation)
	void ChargingChacra();

	UFUNCTION(Server, Reliable, WithValidation)
	void UseChacra();							// Decressed Chacra Count

	UFUNCTION(Server, Reliable, WithValidation)
	void ResetChacraCnt(EChacraActions InputChacraActions, bool bIsUsed = true);

	UFUNCTION()
	FORCEINLINE int8 GetChacraCnt() {return ChacraCnt;}

	// Recover Chacra if Player Die
	UFUNCTION()
	void RecoverChacra();
#pragma endregion

#pragma region PARTICLE&SOUND
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	class USoundBase* ChacraActive_Sound;

	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	FParticles ChacraParticles;

	class UParticleSystemComponent* CurParticle = nullptr;
public:
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void SpawnCurParticle(UParticleSystem* NewParticle);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDestroyCurParticle();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MultiDestroyCurParticle();
#pragma endregion

};
