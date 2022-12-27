#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NPlayerState.generated.h"

UENUM(BlueprintType)
enum class EPlayerCondition : uint8 {
	EPC_Idle			UMETA(DisplayName = "Idle"),
	EPC_Hited			UMETA(DisplayName = "Hited"),
	EPC_Parry			UMETA(DisplayName = "Parry"),
	EPC_Charge			UMETA(DisplayName = "Charge"),
	EPC_Dash			UMETA(DisplayName = "Dash"),
	EPC_Attack			UMETA(DisplayName = "Attack"),

	EPC_Skill			UMETA(DisplayName = "Skill")
};

UCLASS()
class NARUTO_API ANPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ANPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	UFUNCTION()
	FORCEINLINE void SetPlayerCondition(EPlayerCondition NewCondition) { PlayerCondition = NewCondition;}

	UFUNCTION()
	FORCEINLINE EPlayerCondition GetPlayerCondition() {return PlayerCondition; }
protected:
	UPROPERTY(Replicated, VisibleAnywhere, Category="State")
	EPlayerCondition PlayerCondition;
};
