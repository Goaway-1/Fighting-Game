#include "NPlayerState.h"

ANPlayerState::ANPlayerState() {
	Health = 5.f;
}
float ANPlayerState::GetHealth() const{
	return Health;
}
void ANPlayerState::InitPlayerData(){
	Health = 100.f;
}
void ANPlayerState::SetHealth(float val) {
	Health = val;

	OnPlayerHealthChanged.Broadcast();
}