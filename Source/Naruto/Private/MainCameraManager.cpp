#include "MainCameraManager.h"
#include "NarutoGameState.h"
#include "PlayerCharacter.h"
#include "MainPlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

AMainCameraManager::AMainCameraManager() {
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RootComponent = DefaultSceneRoot;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(GetRootComponent());
	SpringArmComp->TargetArmLength = 450.f;
	SpringArmComp->bDoCollisionTest = false;		//OffCollision
	SpringArmComp->bEnableCameraLag = true;
	SpringArmComp->CameraLagSpeed = 20.f;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);
	CameraComp->bUsePawnControlRotation = false;

	ReferenceScene = CreateDefaultSubobject<USceneComponent>(TEXT("ReferenceScene"));
	ReferenceScene->SetupAttachment(GetRootComponent());
	ReferenceScene->SetWorldLocation(FVector(630.f, 0.f, 0.f));

	////new
	//SetReplicates(true);
	//SetReplicateMovement(true);
}
void AMainCameraManager::BeginPlay() {
	Super::BeginPlay();
}
void AMainCameraManager::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	// 플레이어를 찾거나 값을 지정...
	FindAndSet();
}
void AMainCameraManager::FindAndSet() {
	if (PlayerClass && Players.Num() < 2) {
		UGameplayStatics::GetAllActorsOfClass(this, PlayerClass, Players);
	}
	else if (IsLocallyControlled()) {
		SetCameraPosition();
		SetP1RelativeVal();
		if (!bIsTekkenStyle || (!bFixRotate && bIsTekkenStyle)) RotateDefaultScene();
		if (!bIsTekkenStyle) {
			SetReferenceScene();
			SetViewAllPlayers();
			DrawDebugSphere(GetWorld(), ReferenceScene->GetComponentLocation(), 10.f, 10.f, FColor::Green, false, 0.1f);
		}
		DrawDebugSphere(GetWorld(), DefaultSceneRoot->GetComponentLocation(), 10.f, 10.f, FColor::Red, false, 0.1f);
	}
}
void AMainCameraManager::SetReferenceScene() {
	float P1ToCamera_Distance = (Players[0]->GetActorLocation() - CameraComp->GetComponentLocation()).Size();
	float P2ToCamera_Distance = (Players[1]->GetActorLocation() - CameraComp->GetComponentLocation()).Size();
	float P1ToP2_HalfDistance = ((Players[0]->GetActorLocation() - Players[1]->GetActorLocation()).Size()) / 2.f;

	/** 카메라와 더 가까운 플레이어에게 ReferenceScene의 X축 위치를 지정 */
	FVector Vec = (P2ToCamera_Distance > P1ToCamera_Distance) ? FVector(P1ToP2_HalfDistance * -1.f, 0.f, 0.f) : FVector(P1ToP2_HalfDistance, 0.f, 0.f);
	ReferenceScene->SetRelativeLocation(Vec);
}
void AMainCameraManager::RotateDefaultScene() {
	if (bIsTekkenStyle) {
		float AbsInnerVal = FMath::Abs(P1ToRoot_InnerVec);

		if (AbsInnerVal <= 0.98f) {
			float force = (0.98f - AbsInnerVal) * TekkenRotateForce;
			float YawForce = ((-IsForward * IsLeft) * force) * UGameplayStatics::GetWorldDeltaSeconds(this);
			DefaultSceneRoot->AddWorldRotation(FRotator(0.f, YawForce, 0.f), false, false);
		}
	}
	else {
		float P1ToReference_Length = (Players[0]->GetActorLocation() - ReferenceScene->GetComponentLocation()).Size();

		if (P1ToReference_Length >= MinRotDistance) {
			/** 거리에 따른 힘의 세기 */
			float Force = (P1ToReference_Length - MinRotDistance) / RotationDelay;
			float YawForce = ((IsForward * IsLeft) * Force) * UGameplayStatics::GetWorldDeltaSeconds(this);
			DefaultSceneRoot->AddWorldRotation(FRotator(0.f, YawForce, 0.f), false, false);
		}
	}
}
void AMainCameraManager::SetCameraPosition() {
	/** DefaultSceneRoot의 위치 지정 (P1과 P2의 중심위치) */
	FVector DefaultLocation = (Players[0]->GetActorLocation() + Players[1]->GetActorLocation()) / 2.f;
	DefaultLocation.Z += Height;
	DefaultSceneRoot->SetWorldLocation(DefaultLocation);

	/** P1과 P2사이의 거리를 0~1까지의 값으로 제한한 값을 반환 */
	float P1ToP2Distance = (Players[1]->GetActorLocation() - Players[0]->GetActorLocation()).Size() / 1200.f;
	GlobalDistanceFactor = UKismetMathLibrary::FClamp(P1ToP2Distance, 0.f, 1.f);

	/** 거리에 따라 SpringArm의 Pitch를 최소/최대(3~5)로 조절하여 위에서 내려다 보도록 조정 */
	float SpringPitch = UKismetMathLibrary::Lerp(MaxSpringArmRotation, MinSpringArmRotation, GlobalDistanceFactor) * -1.f;
	SpringArmComp->SetRelativeRotation(FRotator(SpringPitch, 0.f, 0.f));

	/** SpringArm의 길이를 지정 */
	FVector P1ToP2Vector = Players[1]->GetActorLocation() - Players[0]->GetActorLocation();
	P1ToP2Vector.Z = 0;
	float P1ToP2CenterLength = (P1ToP2Vector / 2.f).Size();
	float SpringArmLength = UKismetMathLibrary::Lerp(SpringArmBaseDistance, (SpringArmBaseDistance + SpringArmExtraDistance), GlobalDistanceFactor);

	SpringArmComp->TargetArmLength = P1ToP2CenterLength + SpringArmLength;
}
void AMainCameraManager::SetViewAllPlayers() {
	/** 두 플레이어 모두 움직임 없을때 검사 */
	if (Players[0]->GetVelocity().Size() <= 0.1f && Players[1]->GetVelocity().Size() <= 0.1f) {
		float AbsInnerVal = FMath::Abs(P1ToRoot_InnerVec);
		if (AbsInnerVal < MinOverlapInnerVal && !bIsPlayersOverlap) bIsPlayersOverlap = true;

		if (bIsPlayersOverlap) SetNonOverlap();
	}
	else bIsPlayersOverlap = false;
}
void AMainCameraManager::SetNonOverlap() {
	float P1ToRootFactor = UKismetMathLibrary::FClamp((Players[0]->GetActorLocation() - DefaultSceneRoot->GetComponentLocation()).Size() / 800.f, 0.f, 1.f);
	float DeActiveRange = UKismetMathLibrary::Lerp(MaxOverlapInnerVal, MinOverlapInnerVal, P1ToRootFactor);	//내적값의 최소, 최대

	float YawForce = ((IsForward * IsLeft) * (OverlapRotateForce * DeActiveRange)) * UGameplayStatics::GetWorldDeltaSeconds(this);
	DefaultSceneRoot->AddWorldRotation(FRotator(0.f, YawForce, 0.f), false, false);

	/** 일정 내적값을 초과한다면 종료 */
	if (FMath::Abs(P1ToRoot_InnerVec) >= DeActiveRange)  bIsPlayersOverlap = false;
}
void AMainCameraManager::SetP1RelativeVal() {
	FVector P1ToRoot_DirectionVec = UKismetMathLibrary::GetDirectionUnitVector(Players[0]->GetActorLocation(), DefaultSceneRoot->GetComponentLocation());
	P1ToRoot_InnerVec = UKismetMathLibrary::Dot_VectorVector(DefaultSceneRoot->GetRightVector(), P1ToRoot_DirectionVec);

	/** Reference와 방향백터간의 내적을 반환하여->Reference를 기준으로 P1의 좌우 위치 파악 */
	FVector P1ToRef_DirectionVec = UKismetMathLibrary::GetDirectionUnitVector(Players[0]->GetActorLocation(), DefaultSceneRoot->GetComponentLocation());
	float P1ToRef_InnerVec = UKismetMathLibrary::Dot_VectorVector(DefaultSceneRoot->GetRightVector(), P1ToRef_DirectionVec);
	IsLeft = (P1ToRef_InnerVec <= 0.f) ? -1.f : 1.f;		//1 is Left, -1 is Right

	float P1ToCamera_Distance = (Players[0]->GetActorLocation() - CameraComp->GetComponentLocation()).Size();
	float P2ToCamera_Distance = (Players[1]->GetActorLocation() - CameraComp->GetComponentLocation()).Size();
	float P1ToP2_HalfDistance = ((Players[0]->GetActorLocation() - Players[1]->GetActorLocation()).Size()) / 2.f;

	/** 카메라와 더 가까운 플레이어에게 ReferenceScene의 X축 위치를 지정 */
	IsForward = (P2ToCamera_Distance > P1ToCamera_Distance) ? 1.f : -1.f;
}