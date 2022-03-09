#include "NarutoGameState.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

ANarutoGameState::ANarutoGameState() {
	PrimaryActorTick.bCanEverTick = true;

}

void ANarutoGameState::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	FindPlayers();
}

void ANarutoGameState::FindPlayers(){
	/** 플레이어를 찾고 이미 찾았다면 필요한 값들을 계산 */
	if (Players.Num() < 2) UGameplayStatics::GetAllActorsOfClass(GetWorld(), PlayerClass, Players);
	else CalculateCamValues();
}

void ANarutoGameState::CalculateCamValues() {
	/** 두 벡터를 선형 보간하여 그 사이의 중간 벡터를 계산 */
	CenterPos = UKismetMathLibrary::VLerp(Players[0]->GetActorLocation(), Players[1]->GetActorLocation(), 0.5f);
	
	/** 두 벡터 사이의 거리를 계산하고 SpringArm의 거리를 지정 */
	PlayerDis = Players[0]->GetDistanceTo(Players[1]);
	SpringArmLength = FMath::Clamp(MinDis + ((PlayerDis - CalDis) * CalIndex), MinDis, MaxDis);


	DrawDebugSphere(GetWorld(), CenterPos, 10.f, 10.f, FColor::Blue, false, 0.3f);
}