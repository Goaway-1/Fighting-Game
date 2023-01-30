#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NinjaStar.generated.h"

UCLASS()
class NARUTO_API ANinjaStar : public AActor
{
	GENERATED_BODY()
	
public:	
	ANinjaStar();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "NinjaStar", Meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditDefaultsOnly, Category = "NinjaStar", Meta = (AllowPrivateAccess = true))
	class UBoxComponent* BoxCollision;

	UPROPERTY(VisibleAnywhere, Category="NinjaStar | Info")
	AActor* Target;		
	
	UPROPERTY()
	FTimerHandle StopStarHandle;

	bool bIsChacraThrow = false;			// Is Chacra NinjaStar?
	float Speed = 20.f;						
	const float MaxSpeed = 30.f;					
	const float LifeTime = 3.f;
		
private:
	void MoveStar();
	void StopStar();
	void StartThrow();							// Set timer & Collision

	/** If Overlap other Actor */
	UFUNCTION()
	void OnNinjaStarOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	void InitSetting(AActor* player, bool bIsChacra);		// Set Target & Chacra
};
