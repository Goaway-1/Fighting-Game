#include "NCameraManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"
//#include "NPlayer.h"

ANCameraManager::ANCameraManager() {
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RootComponent = DefaultSceneRoot;

	/** SpringArm setting */
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(GetRootComponent());
	SpringArmComp->TargetArmLength = 450.f;
	SpringArmComp->bDoCollisionTest = false;
	SpringArmComp->bEnableCameraLag = true;
	SpringArmComp->CameraLagSpeed = 20.f;

	/** Camera setting */
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);
	CameraComp->bUsePawnControlRotation = false;

	/** ReferceScene setting */
	ReferenceScene = CreateDefaultSubobject<USceneComponent>(TEXT("ReferenceScene"));
	ReferenceScene->SetupAttachment(GetRootComponent());
	ReferenceScene->SetWorldLocation(FVector(630.f, 0.f, 0.f));

	//SERVER
	SetReplicates(true);
	SetReplicateMovement(true);
}
void ANCameraManager::BeginPlay() {
	Super::BeginPlay();

}
void ANCameraManager::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	
	if(Players.Num() >= 2) CalculateVal();
}
void ANCameraManager::CalculateVal() {
	SetCameraPosition();

	/** Rotate the DefaultScene or adjust the ReferenceScene, if the CameraType matches */
	if (GetCameraType() == ECameraType::ECT_3D || GetCameraType() == ECameraType::ECT_2D) {
		SetP1RelativeVal();
		RotateDefaultScene();

		if (!bIsAttackView && GetCameraType() == ECameraType::ECT_3D) {
			SetReferenceScene();
			SetViewAllPlayers();

			/** Unnotate to view ReferenceScene's location. */
			//DrawDebugSphere(GetWorld(), ReferenceScene->GetComponentLocation(), 10.f, 10.f, FColor::Green, false, 0.1f);
		}
	}
	/** Unnotate to view DefaultSceneRoot's location. */
	//DrawDebugSphere(GetWorld(), DefaultSceneRoot->GetComponentLocation(), 10.f, 10.f, FColor::Red, false, 0.1f);		
}
void ANCameraManager::SetReferenceScene() {
	float P1ToCamera_Distance = (Players[0]->GetActorLocation() - CameraComp->GetComponentLocation()).Size();
	float P2ToCamera_Distance = (Players[1]->GetActorLocation() - CameraComp->GetComponentLocation()).Size();
	float P1ToP2_HalfDistance = ((Players[0]->GetActorLocation() - Players[1]->GetActorLocation()).Size()) / 2.f;

	/** Place the X-axis of the ReferenceScene around the player closer to the camera */
	FVector Vec = (P2ToCamera_Distance > P1ToCamera_Distance) ? FVector(P1ToP2_HalfDistance * -1.f, 0.f, 0.f) : FVector(P1ToP2_HalfDistance, 0.f, 0.f);
	ReferenceScene->SetRelativeLocation(Vec);
}
void ANCameraManager::RotateDefaultScene() {
	if(GetCameraType() != ECameraType::ECT_3D || bIsAttackView) {
		float AbsInnerVal = FMath::Abs(P1ToRoot_InnerVec);

		if (AbsInnerVal <= 0.9f) {
			float Force = (0.98f - AbsInnerVal) * RotateForce;
			float YawForce = ((-IsForward * IsLeft) * Force) * UGameplayStatics::GetWorldDeltaSeconds(this);
			DefaultSceneRoot->AddWorldRotation(FRotator(0.f, YawForce, 0.f), false, false);
		}
		else {
			bIsAttackView = false;
		}
	}
	else if (GetCameraType() == ECameraType::ECT_3D) {
		float P1ToReference_Length = (Players[0]->GetActorLocation() - ReferenceScene->GetComponentLocation()).Size();

		/** If the P1ToReference_Length is bigger than MinRotDistance, rotate DefaultSceneRoot */
		if (P1ToReference_Length >= MinRotDistance) {
			/** The strength of the force increases with distance. */
			float Force = (P1ToReference_Length - MinRotDistance) / RotationDelay;
			float YawForce = ((IsForward * IsLeft) * Force) * UGameplayStatics::GetWorldDeltaSeconds(this);
			DefaultSceneRoot->AddWorldRotation(FRotator(0.f, YawForce, 0.f), false, false);
		}
	}
}
void ANCameraManager::SetCameraPosition() {
	/** Positioning of the DefaultSceneRoot */
	FVector DefaultLocation = (Players[0]->GetActorLocation() + Players[1]->GetActorLocation()) / 2.f;
	DefaultLocation.Z += Height;
	DefaultSceneRoot->SetWorldLocation(DefaultLocation);

	/** Specify the value of the GlobalDistanceFactor (P1 to P2) */
	float P1ToP2Distance = (Players[1]->GetActorLocation() - Players[0]->GetActorLocation()).Size() / 1200.f;
	GlobalDistanceFactor = UKismetMathLibrary::FClamp(P1ToP2Distance, 0.f, 1.f);

	/** Adjust the Pitch on the SpringArm according to the distance */
	float SpringPitch = UKismetMathLibrary::Lerp(MaxSpringArmRotation, MinSpringArmRotation, GlobalDistanceFactor) * -1.f;
	SpringArmComp->SetRelativeRotation(FRotator(SpringPitch, 0.f, 0.f));

	/** Specify the length of the SpringArm */
	FVector P1ToP2Vector = Players[1]->GetActorLocation() - Players[0]->GetActorLocation();
	P1ToP2Vector.Z = 0;
	float P1ToP2CenterLength = (P1ToP2Vector / 2.f).Size();
	float SpringArmLength = UKismetMathLibrary::Lerp(SpringArmBaseDistance, (SpringArmBaseDistance + SpringArmExtraDistance), GlobalDistanceFactor);

	SpringArmComp->TargetArmLength = P1ToP2CenterLength + SpringArmLength;
}
void ANCameraManager::SetViewAllPlayers() {
	/** If neither player has movement, check overlap */
	if (Players[0]->GetVelocity().Size() <= 0.1f && Players[1]->GetVelocity().Size() <= 0.1f) {
		float AbsInnerVal = FMath::Abs(P1ToRoot_InnerVec);
		if (AbsInnerVal < MinOverlapInnerVal && !bIsPlayersOverlap) bIsPlayersOverlap = true;

		/** Active SetNonOverlap() if Overlap*/
		if (bIsPlayersOverlap) SetNonOverlap();
	}
	else bIsPlayersOverlap = false;
}
void ANCameraManager::SetNonOverlap() {
	float P1ToRootFactor = UKismetMathLibrary::FClamp((Players[0]->GetActorLocation() - DefaultSceneRoot->GetComponentLocation()).Size() / 800.f, 0.f, 1.f);
	float DeActiveRange = UKismetMathLibrary::Lerp(MaxOverlapInnerVal, MinOverlapInnerVal, P1ToRootFactor);

	float YawForce = ((IsForward * IsLeft) * (OverlapRotateForce * DeActiveRange)) * UGameplayStatics::GetWorldDeltaSeconds(this);
	DefaultSceneRoot->AddWorldRotation(FRotator(0.f, YawForce, 0.f), false, false);

	/** Exits if it exceeds the internal value of the schedule */
	if (FMath::Abs(P1ToRoot_InnerVec) >= DeActiveRange)  bIsPlayersOverlap = false;
}
void ANCameraManager::SetP1RelativeVal() {
	FVector P1ToRoot_DirectionVec = UKismetMathLibrary::GetDirectionUnitVector(Players[0]->GetActorLocation(), DefaultSceneRoot->GetComponentLocation());
	P1ToRoot_InnerVec = UKismetMathLibrary::Dot_VectorVector(DefaultSceneRoot->GetRightVector(), P1ToRoot_DirectionVec);

	/** Identify the left(1) and right(-1) positions of P1 based on the ReferenceScene */
	FVector P1ToRef_DirectionVec = UKismetMathLibrary::GetDirectionUnitVector(Players[0]->GetActorLocation(), ReferenceScene->GetComponentLocation());
	float P1ToRef_InnerVec = UKismetMathLibrary::Dot_VectorVector(ReferenceScene->GetRightVector(), P1ToRef_DirectionVec);
	IsLeft = (P1ToRef_InnerVec <= 0.f) ? -1.f : 1.f;

	float P1ToCamera_Distance = (Players[0]->GetActorLocation() - CameraComp->GetComponentLocation()).Size();
	float P2ToCamera_Distance = (Players[1]->GetActorLocation() - CameraComp->GetComponentLocation()).Size();
	float P1ToP2_HalfDistance = ((Players[0]->GetActorLocation() - Players[1]->GetActorLocation()).Size()) / 2.f;

	/** Compare if P1 precedes P2 */
	IsForward = (P2ToCamera_Distance > P1ToCamera_Distance) ? 1.f : -1.f;
}
void ANCameraManager::SetPlayer(AActor* Player) {
	if (!HasAuthority()) {
		ServerSetPlayer(Player);
	}

	Players.Push(Player);
}
void ANCameraManager::ServerSetPlayer_Implementation(AActor* Player) {
	SetPlayer(Player);
}
bool ANCameraManager::ServerSetPlayer_Validate(AActor* Player) {
	return true;
}
void ANCameraManager::SetAttackView() {
	if (GetCameraType() == ECameraType::ECT_3D) {
		bIsAttackView = true;
	}
}
void ANCameraManager::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANCameraManager, Players);
}