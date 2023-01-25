#include "NPlayerState.h"
#include "NPlayer.h"
#include "NPlayerController.h"

ANPlayerState::ANPlayerState() {
	InitPlayerData();
}
float ANPlayerState::GetState(EWidgetState state) const{
	switch (state)
	{
	case EWidgetState::EWS_Health:
		return Health;
	case EWidgetState::EWS_Chacra:
		return Chacra;
	case EWidgetState::EWS_Switch:
		return (float)SideStep;
	case EWidgetState::EWS_Score:
		return (float)Score;
	default:
		return Chacra;
	}
}
void ANPlayerState::InitPlayerData(){
	Health = 100.f;
	Chacra = 100.f;
	SideStep = 4;
	Score = 0;
}
void ANPlayerState::SetState(EWidgetState state, float val) {
	switch (state)
	{
	case EWidgetState::EWS_Health:
		Health = val;
		break;
	case EWidgetState::EWS_Chacra:
		Chacra = val;
		break;
	case EWidgetState::EWS_Switch:
		SideStep = (int)val;
		break;
	case EWidgetState::EWS_Score:
		Score = (int)val;
		break;
	default:
		break;
	}
}