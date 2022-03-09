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
	/** �÷��̾ ã�� �̹� ã�Ҵٸ� �ʿ��� ������ ��� */
	if (Players.Num() < 2) UGameplayStatics::GetAllActorsOfClass(GetWorld(), PlayerClass, Players);
	else CalculateCamValues();
}

void ANarutoGameState::CalculateCamValues() {
	/** �� ���͸� ���� �����Ͽ� �� ������ �߰� ���͸� ��� */
	CenterPos = UKismetMathLibrary::VLerp(Players[0]->GetActorLocation(), Players[1]->GetActorLocation(), 0.5f);
	
	/** �� ���� ������ �Ÿ��� ����ϰ� SpringArm�� �Ÿ��� ���� */
	PlayerDis = Players[0]->GetDistanceTo(Players[1]);
	SpringArmLength = FMath::Clamp(MinDis + ((PlayerDis - CalDis) * CalIndex), MinDis, MaxDis);


	DrawDebugSphere(GetWorld(), CenterPos, 10.f, 10.f, FColor::Blue, false, 0.3f);
}