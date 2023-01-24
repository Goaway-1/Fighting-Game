#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NinjaStar.generated.h"

/***
dd
*/

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

#pragma region STAR
protected:
	UPROPERTY(EditAnywhere, Category = "Info", Meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere, Category = "Info", Meta = (AllowPrivateAccess = true))
	class UBoxComponent* BoxCollision;

	UPROPERTY(VisibleAnywhere, Category="Info")
	AActor* Target;		
	
	UPROPERTY()
	FTimerHandle StopStarHandle;

	UPROPERTY()
	bool bIsChacraThrow = false;

	float Speed = 20.f;						// Move Speed
	float MaxSpeed = 30.f;					// Move Speed (Chacra)
	float LifeTime = 3.f;					// Life Time
		
private:
	UFUNCTION()
	void MoveStar();

	UFUNCTION()
	void StopStar();

	void Setting();							// Set timer & Collision

	/** If Overlap other Actor */
	UFUNCTION()
	void OnNinjaStarOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
public:
	UFUNCTION()
	void InitSetting(AActor* player, bool bIsChacra);		// Set Target & Chacra
#pragma endregion
};
