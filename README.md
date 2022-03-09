# 3D Fighting Game
- 3D Fighting Game (reproduction of Naruto Shippuden)
- 나루토 얼티밋스톰4를 기준으로 모작

## **Day_1**
> **<h3>Today Dev Story</h3>**
- ## <span style = "color:yellow;">캐릭터의 이동</span>
  - 캐릭터의 기본 이동은 카메라의 시점을 기준으로 상하좌우를 구분하여 이동 (방향키를 사용)
  - Camera는 존재하나 SpringArm은 존재하지 않고 BeginPlay 시점에 MainCameraManager클래스의 SpringArm에 Attach하여 사용
    - GetActorOfClass를 통해서 월드에 존재하는 원하는 클래스 타입을 검색하고 그 액터의 SpringArm에 AttachToComponent하여 부착
   
    <details><summary>C++ File</summary> 

    ```c++
    //PlayerCharacter.cpp
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
    void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent){
      Super::SetupPlayerInputComponent(PlayerInputComponent);

      /** 이동 */
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
    ```
    </details>
    <details><summary>Header File</summary> 

    ```c++
    //PlayerCharacter.h  
    private:
      /** Actor Move */
      void MoveForward(float Value);
      void MoveRight(float Value);

      /**  */
      void AttachCamToManager();
    protected:
      UPROPERTY(EditDefaultsOnly, Category = "Components")
      TSubclassOf<AActor> SpectatingViewpointClass;		//Find MainCameraManager Class...

      UPROPERTY(EditDefaultsOnly, Category = "Components")
      UCameraComponent* CameraComp;
    ```
    </details>

- ## <span style = "color:yellow;">메인 카메라의 이동</span>CameraLogic.png
  - <img src="Image/Camera(Length,Center).gif" height="300" title="Camera(Length,Center)">
  - AGameStateBase클래스를 상속받은 NarutoGameState클래스를 생성하고, SpringArm의 길이를 고정하는 로직구현
    - 매 틱마다 FindPlayers()메서드를 통해 지정한 클래스의 플레이어를 찾고 찾았다면 CalculateCamValue()메서드를 통해 추후 사용될 정보를 계산
    - 두 벡터 사이의 중간벡터를 위해서 선형 보간(VLerp())메서드를 사용하여 벡터 값인 CenterPos를 계산
    - 두 벡터 사이의 거리를 GetDistanceTo()메서드를 사용하여 float값인 PlayerDis를 계산하고 이를 사용하여 SpringArmLength를 계산
      - SpringArmLength = 최소거리 + ((사이의 거리 - 특정 값) * r값)이고, Clamp를 사용하여 최소/최대 넘지 않도록 설정
  - Actor클래스를 상속받은 __MainCameraManager클래스를__ 생성하고 이 클래스는 플레이어들의 시점이 되어줄 액터
    - <img src="Image/CameraLogic.png" height="300" title="CameraLogic">
    - 위 그림과 같은 로직으로 MeshComponent는 항상 두 플레이어 사이의 중심에 위치하고 SpringArm에는 Scene가 Attach되어 있는 구조
    - SceneComp에는 플레이어들의 Camera가 Attach되는 위치
    - 매 틱마다 GameState가 계산한 CenterPos, SpringArmLength를 통하여 MeshComp의 위치와 SpringArmComp의 길이를 조절

    <details><summary>C++ File</summary> 

    ```c++
    //NarutoGameState.cpp
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
    ```
    ```c++
    //MainCameraManager.cpp
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
    ```
    </details>

    <details><summary>Header File</summary> 

    ```c++
    //NarutoGameState.h
    private:
      UPROPERTY(VisibleAnywhere, Category = "ForCamera")
      FVector CenterPos;						//Intermediate vector between players (Used in MainCameraManager)

      UPROPERTY(VisibleAnywhere, Category = "ForCamera")
      float PlayerDis;						//Distance Between Players

      UPROPERTY(VisibleAnywhere, Category = "ForCamera")
      float SpringArmLength;					//The Length of SpringArm (Used in MainCameraManager)

      //Calculate of Value
      const float MinDis = 450.f;
      const float MaxDis = 5000000.f;
      const float CalDis = 500.f;
      const float CalIndex = 0.8;

      UPROPERTY(VisibleAnywhere, Category = "ForCamera")
      TArray<AActor*> Players;				//Players who found it
      
    protected:
      UPROPERTY(EditAnywhere, Category = "ForCamera")
      TSubclassOf<AActor> PlayerClass;		//Find Type

    public:
      // Find Players and calculate the vector
      void FindPlayers();
      void CalculateCamValues();

      FORCEINLINE FVector GetCenterPos() { return CenterPos; }
      FORCEINLINE float GetSpringArmLength() { return SpringArmLength; } 
    ```
    ```c++
    //MainCameraManager.h
    private:
      UPROPERTY(VisibleAnywhere, Category = "GameState")
      class ANarutoGameState* GS;

    protected:
      UPROPERTY(EditDefaultsOnly, Category = "Components")
      class UStaticMeshComponent* MeshComp;

      UPROPERTY(EditDefaultsOnly, Category = "Components")
      class USceneComponent* SceneComp;

      UPROPERTY(EditDefaultsOnly, Category = "Components")
      class USpringArmComponent* SpringArmComp;

    public:
      FORCEINLINE USpringArmComponent* GetSpringArm() {return SpringArmComp;}
    ```
    </details>

**<h3>Realization</h3>**
  - null