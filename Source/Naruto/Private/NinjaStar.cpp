#include "NinjaStar.h"
#include "NPlayer.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Components/BoxComponent.h"

ANinjaStar::ANinjaStar(){
 	PrimaryActorTick.bCanEverTick = true;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetIsReplicated(true);
	SetRootComponent(BoxCollision);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(RootComponent);

	bReplicates = true;
	SetReplicateMovement(true);
}
void ANinjaStar::BeginPlay(){
	Super::BeginPlay();

	Setting();
}
void ANinjaStar::Tick(float DeltaTime){
	Super::Tick(DeltaTime);

	MoveStar();
	StaticMesh->AddLocalRotation(FRotator(0.f,5.f,0.f));
}
void ANinjaStar::Setting() {
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ANinjaStar::OnNinjaStarOverlapBegin);

	GetWorld()->GetTimerManager().ClearTimer(StopStarHandle);
	GetWorld()->GetTimerManager().SetTimer(StopStarHandle, this, &ANinjaStar::StopStar, LifeTime, false);
}
void ANinjaStar::InitSetting(AActor *player, bool bIsChacra) {
	Target = player;
	bIsChacraThrow = bIsChacra;

	/** Set Speed! */
	if(bIsChacra) {Speed = MaxSpeed; UE_LOG(LogTemp, Warning, TEXT("PowerFull NinjaStar!!"));}
}
void ANinjaStar::MoveStar() {
	if (Target) {
		SetActorLocation(GetActorTransform().GetLocation() + (GetActorForwardVector() * Speed));

		/** Set Rotation if ChacraThrow.. */
		if (bIsChacraThrow) {			
			FRotator Rot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target->GetActorLocation());

			/* Get gap between two players..
			* move Yaw&Pitch if not a straight line */
			int YawVal = GetActorRotation().Yaw - Rot.Yaw, PitchVal = GetActorRotation().Pitch - Rot.Pitch;
			YawVal = (abs(YawVal) > 1) ? ((YawVal < 0) ? 1 : -1) : 0;
			PitchVal = (abs(PitchVal) > 1) ? ((PitchVal < 0) ? 1 : -1) : 0;

			AddActorLocalRotation(FRotator(PitchVal, YawVal,0.f));
		}
	}
}
void ANinjaStar::StopStar() {
	UE_LOG(LogTemp, Warning, TEXT("NinjaStar is Destory..."));
	Destroy();
}
void ANinjaStar::OnNinjaStarOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (OtherActor != Owner) {
		UE_LOG(LogTemp, Warning, TEXT("NinjaStar is Hited %s"), *OtherActor->GetName());

		ANPlayer* victim = Cast<ANPlayer>(OtherActor);
		victim->IsHited(EPlayerCondition::EPC_Attack ,0);
		StopStar();
	}
}