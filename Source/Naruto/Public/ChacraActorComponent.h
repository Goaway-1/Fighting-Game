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

public:
	UFUNCTION()
	void UseChacra();

	UFUNCTION()
	void ResetChacraCnt();

	UFUNCTION()
	FORCEINLINE int8 GetChacraCnt() {return ChacraCnt;}
#pragma endregion
};
