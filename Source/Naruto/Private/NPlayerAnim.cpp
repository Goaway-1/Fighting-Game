#include "NPlayerAnim.h"
#include "NPlayer.h"

void UNPlayerAnim::NativeInitializeAnimation() {
	if (!Player) Player = Cast<ANPlayer>(TryGetPawnOwner());
}
void UNPlayerAnim::NativeUpdateAnimation(float DeltaSeconds) {
	Super::NativeUpdateAnimation(DeltaSeconds);

}