#pragma once

#include "EngineMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "NarutoGameState.generated.h"

UCLASS()
class NARUTO_API ANarutoGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ANarutoGameState();

	virtual void Tick(float DeltaSeconds) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "ForCamera")
	FVector CenterPos;						//Intermediate vector between players (Used in MainCameraManager)

	UPROPERTY(VisibleAnywhere, Category = "ForCamera")
	float PlayerDis;						//Distance Between Players

	UPROPERTY(VisibleAnywhere, Category = "ForCamera")
	float SpringArmLength;					//The Length of SpringArm (Used in MainCameraManager)

	//Calculate of Value
	const float MinDis = 450.f;
	const float MaxDis = 5000000.f;
	const float CalDis = 500.f;
	const float CalIndex = 0.8;

	UPROPERTY(VisibleAnywhere, Category = "ForCamera")
	TArray<AActor*> Players;				//Players who found it
protected:
	UPROPERTY(EditAnywhere, Category = "ForCamera")
	TSubclassOf<AActor> PlayerClass;		//Find Type

public:
	// Find Players and calculate the vector
	void FindPlayers();
	void CalculateCamValues();

	FORCEINLINE FVector GetCenterPos() { return CenterPos; }
	FORCEINLINE float GetSpringArmLength() { return SpringArmLength; } 
};
