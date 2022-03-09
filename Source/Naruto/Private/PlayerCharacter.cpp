#include "PlayerCharacter.h"
#include "MainCameraManager.h"

APlayerCharacter::APlayerCharacter(){
 	PrimaryActorTick.bCanEverTick = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(GetRootComponent());
	CameraComp->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationPitch = false;

	/** 이동방향으로 자동회전 */
	GetCharacterMovement()->bOrientRotationToMovement = true;	
	GetCharacterMovement()->RotationRate = FRotator(0.f, 2000.f, 0.f);
}

void APlayerCharacter::BeginPlay(){
	Super::BeginPlay();

	/** 카메라를 부착하기 위한 과정 */
	AttachCamToManager();
}

void APlayerCharacter::Tick(float DeltaTime){
	Super::Tick(DeltaTime);

}
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent){
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	/** Move */
	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
}
void APlayerCharacter::MoveForward(float Value) {
	AddMovementInput(CameraComp->GetForwardVector(), Value);
}
void APlayerCharacter::MoveRight(float Value) {
	AddMovementInput(CameraComp->GetRightVector(), Value);
}
void APlayerCharacter::AttachCamToManager() {
	if (SpectatingViewpointClass) {
		AMainCameraManager* TargetCamera = Cast<AMainCameraManager>(UGameplayStatics::GetActorOfClass(this, SpectatingViewpointClass));

		// 액터가 있다면 뷰타겟을 변경
		if (TargetCamera) {
			FLatentActionInfo LatentInfo;
			CameraComp->AttachToComponent(TargetCamera->GetSpringArm(), FAttachmentTransformRules(EAttachmentRule::KeepWorld, false));
			LatentInfo.CallbackTarget = this;
			UKismetSystemLibrary::MoveComponentTo(CameraComp, FVector(0.f), FRotator(0.f), false, false, 0.3f, true, EMoveComponentAction::Type::Move, LatentInfo);
		}
	}
}