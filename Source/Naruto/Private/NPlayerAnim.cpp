#include "NPlayerAnim.h"
#include "NPlayer.h"

void UNPlayerAnim::NativeInitializeAnimation() {
	if (!Player) Player = Cast<ANPlayer>(TryGetPawnOwner());
}
void UNPlayerAnim::NativeUpdateAnimation(float DeltaSeconds) {
	Super::NativeUpdateAnimation(DeltaSeconds);

	/*if (Player) {
		FVector Speed = Player->GetVelocity();
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.f);
		MovementSpeed = LateralSpeed.Size();

		//bisjumping = Player->GetMovementComponent()->IsFalling();
	}*/
}