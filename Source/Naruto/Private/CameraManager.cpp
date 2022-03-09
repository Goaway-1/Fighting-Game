#include "CameraManager.h"

UCameraManager::UCameraManager(){
	PrimaryComponentTick.bCanEverTick = true;

}

void UCameraManager::BeginPlay(){
	Super::BeginPlay();

}
void UCameraManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction){
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}