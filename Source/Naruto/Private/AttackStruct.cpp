#include "AttackStruct.h"

AAttackStruct::AAttackStruct(){
	PrimaryActorTick.bCanEverTick = false;

}
void AAttackStruct::BeginPlay(){
	Super::BeginPlay();
	
}
void AAttackStruct::Tick(float DeltaTime){
	Super::Tick(DeltaTime);

}

