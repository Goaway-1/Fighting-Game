#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NinjaStar.generated.h"

/***
- 사용자의 전면에 생성되며, 지속적으로 갱신되는 목표물의 위치로 이동한다.
- 이때 목표물의 위치로 이동할때, 직선으로만 이동하며 회전을 통해 방향을 전환한다.
  - 회전에는 한계가 있기 때문에 목표물에 도달하지 못할 수 있다.
- 부딪치거나, 일정 시간이 지나면 표창은 사라지며, 이때 다시 시전할 수 있다.
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
