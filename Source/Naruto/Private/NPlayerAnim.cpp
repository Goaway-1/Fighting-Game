#include "NPlayerAnim.h"
#include "NPlayer.h"
//#include "Net/UnrealNetwork.h"

void UNPlayerAnim::NativeInitializeAnimation() {
	if (!Player) Player = Cast<ANPlayer>(TryGetPawnOwner());
}
void UNPlayerAnim::NativeUpdateAnimation(float DeltaSeconds) {
	Super::NativeUpdateAnimation(DeltaSeconds);

}

//void UNPlayerAnim::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//	DOREPLIFETIME(UNPlayerAnim, Player);
//}