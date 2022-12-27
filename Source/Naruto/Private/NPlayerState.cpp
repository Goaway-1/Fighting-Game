#include "NPlayerState.h"
#include "Net/UnrealNetwork.h"

ANPlayerState::ANPlayerState() {
	SetPlayerCondition(EPlayerCondition::EPC_Idle);
}

void ANPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANPlayerState, PlayerCondition);
}