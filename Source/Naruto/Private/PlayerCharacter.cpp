#include "PlayerCharacter.h"
#include "MainPlayerController.h"
#include "MainCameraManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

APlayerCharacter::APlayerCharacter(){
 	PrimaryActorTick.bCanEverTick = true;

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

void APlayerCharacter::PossessedBy(AController* NewController) {
	Super::PossessedBy(NewController);

	PlayerController = Cast<AMainPlayerController>(GetController());

}

void APlayerCharacter::Tick(float DeltaTime){
	Super::Tick(DeltaTime);


	if (IsLocallyControlled() && CameraManager && PlayerController) {
		FRotator NRot = FRotator(CameraManager->GetActorRotation().Pitch, CameraManager->GetActorRotation().Yaw, GetController()->GetControlRotation().Roll);
		GetController()->SetControlRotation(NRot);
	}

	if(IsLocallyControlled() && PlayerController && CameraManager && !isDone) {
		PlayerController->SetViewTargetWithBlend(CameraManager);
		isDone = true;
	}
}
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent){
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	/** Move */
	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
}
void APlayerCharacter::MoveForward(float Value) {
	FRotator Rot = FRotator(0.f, GetControlRotation().Yaw,0.f);
	AddMovementInput(UKismetMathLibrary::GetForwardVector(Rot), Value);
}
void APlayerCharacter::MoveRight(float Value) {
	FRotator Rot = FRotator(0.f, GetControlRotation().Yaw, 0.f);
	AddMovementInput(UKismetMathLibrary::GetRightVector(Rot), Value);
}
void APlayerCharacter::AttachCamToManager() {
	if (SpectatingViewpointClass) {
		AMainCameraManager* TargetCamera = Cast<AMainCameraManager>(UGameplayStatics::GetActorOfClass(this, SpectatingViewpointClass));

		// 액터가 있다면 뷰타겟을 변경
		if (TargetCamera) CameraManager = TargetCamera;
	}
}