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

	/** �̵��������� �ڵ�ȸ�� */
	GetCharacterMovement()->bOrientRotationToMovement = true;	
	GetCharacterMovement()->RotationRate = FRotator(0.f, 2000.f, 0.f);
}

void APlayerCharacter::BeginPlay(){
	Super::BeginPlay();

	/** ī�޶� �����ϱ� ���� ���� */
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

		// ���Ͱ� �ִٸ� ��Ÿ���� ����
		if (TargetCamera) {
			FLatentActionInfo LatentInfo;
			CameraComp->AttachToComponent(TargetCamera->GetSpringArm(), FAttachmentTransformRules(EAttachmentRule::KeepWorld, false));
			LatentInfo.CallbackTarget = this;
			UKismetSystemLibrary::MoveComponentTo(CameraComp, FVector(0.f), FRotator(0.f), false, false, 0.3f, true, EMoveComponentAction::Type::Move, LatentInfo);
		}
	}
}