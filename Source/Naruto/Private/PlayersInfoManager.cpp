#include "PlayersInfoManager.h"
#include "Net/UnrealNetwork.h"

APlayersInfoManager::APlayersInfoManager(){
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);

	P1Health = 100.f;			//BP_Players0
	P2Health = 100.f;
}
void APlayersInfoManager::BeginPlay(){
	Super::BeginPlay();
	
}
void APlayersInfoManager::Tick(float DeltaTime){
	Super::Tick(DeltaTime);

}
void APlayersInfoManager::SetState(int idx, float Health) {	
	if(idx == 1) P1Health = Health;
	else P2Health = Health;
}
float APlayersInfoManager::GetState(int idx) {
	return (idx == 1) ? P1Health : P2Health;
}
void APlayersInfoManager::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayersInfoManager, P1Health);
	DOREPLIFETIME(APlayersInfoManager, P2Health);
}
