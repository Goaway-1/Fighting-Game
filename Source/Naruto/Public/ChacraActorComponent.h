#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ChacraActorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NARUTO_API UChacraActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UChacraActorComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma region CHACRA
protected:
	UPROPERTY(Replicated, VisibleAnywhere)
	int8 ChacraCnt;

	UPROPERTY()
	FTimerHandle ResetChacraHandle;

	UPROPERTY()
	TWeakObjectPtr<class ANPlayer> OwnPlayer;

private:
	const float MaxChacra = 100.f;
	float ChacraRatio = 0.f;
	const float ChargingVal = 0.7f;				// Increase Val

	UPROPERTY(Replicated, VisibleAnywhere, Category = "Chacra")
	float CurrentChacra = 100.f;

public:
	FORCEINLINE void SetChacraRatio() { ChacraRatio = CurrentChacra / MaxChacra; }		

	UFUNCTION()
	FORCEINLINE float GetChacraRatio() { return ChacraRatio; }	

	FORCEINLINE float GetChacra() { return CurrentChacra; }

	UFUNCTION(Server, Reliable, WithValidation)
	void ChargingChacra();

	UFUNCTION(Server, Reliable, WithValidation)
	void UseChacra();				// Decressed Chacra

	UFUNCTION(Server, Reliable, WithValidation)
	void ResetChacraCnt(bool bIsUsed = true);

	UFUNCTION()
	FORCEINLINE int8 GetChacraCnt() {return ChacraCnt;}
#pragma endregion
};
