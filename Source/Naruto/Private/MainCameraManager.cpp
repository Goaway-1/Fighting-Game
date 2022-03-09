#include "MainCameraManager.h"
#include "NarutoGameState.h"

AMainCameraManager::AMainCameraManager(){
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(GetRootComponent());
	SpringArmComp->TargetArmLength = 450.f;
	SpringArmComp->bDoCollisionTest = false;		//OffCollision
	SpringArmComp->bEnableCameraLag = true;
	SpringArmComp->CameraLagSpeed = 20.f;

	/*CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);
	CameraComp->bUsePawnControlRotation = false;*/

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	SceneComp->SetupAttachment(SpringArmComp);
}

void AMainCameraManager::BeginPlay(){
	Super::BeginPlay();
	
	GS = GetWorld()->GetGameState<ANarutoGameState>();
}

void AMainCameraManager::Tick(float DeltaTime){
	Super::Tick(DeltaTime);

	if (GS) {
		SetActorLocation(GS->GetCenterPos());
		SpringArmComp->TargetArmLength = GS->GetSpringArmLength();
	}
}