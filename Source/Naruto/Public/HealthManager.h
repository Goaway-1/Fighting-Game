#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NARUTO_API UHealthManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHealthManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma region HEALTH
private:
	/** Health */
	const float MaxHealth = 100.f;

	UPROPERTY(Replicated, VisibleAnywhere, Category="Health")
	float CurrentHealth = 100.f;

	UPROPERTY()
	float HealthRatio = 0.f;

public:
	/** Health */
	FORCEINLINE void SetHealthRatio(){	HealthRatio = CurrentHealth / MaxHealth; }

	UFUNCTION()
	void SetDecreaseHealth(float val);			// Decrease Health & Return true if Health is under than 0

	UFUNCTION()
	bool GetIsDead();

	UFUNCTION()
	FORCEINLINE float GetHealthRatio() { return HealthRatio; }

	UFUNCTION()
	FORCEINLINE float GetCurrentHealth() { return CurrentHealth; }
#pragma endregion
};
