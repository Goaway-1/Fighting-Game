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

#pragma region HEALTH
private:
	const int8 MaxHealth = 100;

	UPROPERTY(Replicated, VisibleAnywhere, Category="Health")
	int8 CurrentHealth;
public:
	UFUNCTION()
	void SetDecreaseHealth(int8 val);			// Decrease Health & Return true if Health is under than 0

	FORCEINLINE float GetCurrentHealth() { return CurrentHealth; }

	void RecoverHealth();						// Recover Player Health if Player Die

	bool GetIsDead();
#pragma endregion
};
