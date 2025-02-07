#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "NPlayerAnim.generated.h"

UCLASS()
class NARUTO_API UNPlayerAnim : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY(BlueprintReadOnly, Category = "Movement", Meta = (AllowPrivateAccess = true))
	class ANPlayer* Player;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", Meta = (AllowPrivateAccess = true))
	float MovementSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", Meta = (AllowPrivateAccess = true))
	bool bisjumping = false;
};
