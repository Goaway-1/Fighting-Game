# 3D Fighting Game
- 3D Fighting Game (reproduction of Naruto Shippuden)
- 나루토 얼티밋스톰4를 기준으로 모작

## **Day_1**
> **<h3>Today Dev Story</h3>**
- ## <span style = "color:yellow;">메인 카메라의 이동</span>
  - <img src="Image/MainCamera_server.gif" height="300" title="MainCamera_server">
  - 기존 본인이 제작한 "CameraManager" plugin을 서버 멀티플레이에서도 동작하도록 수정 및 개선
  - __플레이어__
    - 월드 상의 NcameraManager를 찾아 __"ClientSetViewTarget"하여__ 컨트롤러의 타겟을 카메라로 변경
    - CameraManager의 SetPlayer()메서드를 통해, 본인 객체를 __CamerManager가 참고하도록 설정__
  - __메인 카메라__
    - Player들의 정보가 저장되는 Players라는 TArray를 __Replicated하여__ 변경되면 서버에도 똑같이 반영
    - 그를 위해 __SetPlayer()메서드가__ Server에서 실행되는 __ServerSetPlayer()메서드__ 선언 후 구현
  - __문제점__
    - Server모드에서만 적용되는 문제 존재
    - 반응 속도가 느리다는 문제 (쉽게 해결 가능)

    <details><summary>C++ File</summary> 

    ```c++
    //NPlayer.cpp
    void ANPlayer::BeginPlay() {
      Super::BeginPlay();
      
      /** Find MainCameraManager & Set */
      if (CameraManagerClass) {
        ANCameraManager* TargetCamera = Cast<ANCameraManager>(UGameplayStatics::GetActorOfClass(this, ANCameraManager::StaticClass()));

        if (TargetCamera) {
          CameraManager = TargetCamera;
          APlayerController* ControllComp = Cast<APlayerController>(GetController());
          if (ControllComp) {
            PlayerControlComp = ControllComp;
            PlayerControlComp->bAutoManageActiveCameraTarget = false;
            PlayerControlComp->ClientSetViewTarget(CameraManager);
          }
        }
        else GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Error! MainCameraManager does not exist in the world!"));
      }

      if (CameraManager) CameraManager->SetPlayer(this);
    }
    ```
    ```c++
    //NCameraManager.cpp
    #include "Net/UnrealNetwork.h"

    ANCameraManager::ANCameraManager() {  
      SetReplicates(true);
      SetReplicateMovement(true);
    }
    void ANCameraManager::SetPlayer(AActor* Player) {
      if (!HasAuthority()) ServerSetPlayer(Player);

      UE_LOG(LogTemp, Warning, TEXT("New Player : %s"), *Player->GetName());
      Players.Push(Player);
    }
    void ANCameraManager::ServerSetPlayer_Implementation(AActor* Player) {
      SetPlayer(Player);
    }
    bool ANCameraManager::ServerSetPlayer_Validate(AActor* Player) {
      return true;
    }
    void ANCameraManager::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const{
      Super::GetLifetimeReplicatedProps(OutLifetimeProps);

      DOREPLIFETIME(ANCameraManager, Players);
    }
    ```
    
    </details>

    <details><summary>Header File</summary> 

    ```c++
    //NCameraManager.h	
    public:
      void SetPlayer(AActor* Player);

      UFUNCTION(Server, Reliable, WithValidation)
      void ServerSetPlayer(AActor* Player);
    protected:
    	UPROPERTY(Replicated, EditInstanceOnly, Category = "Setting")
	    TArray<AActor*> Players;
    ```
    </details>

**<h3>Realization</h3>**
  - 메인카메라가 Server모드에서만 적용되는 문제 존재, 수정 필요..!
  - 반응 속도가 느리다는 문제 (쉽게 해결 가능)

## **Day_2**
> **<h3>Today Dev Story</h3>**
- ## <span style = "color:yellow;">애니메이션</span>
  - <img src="Image/BP_Animation.png" height="300" title="BP_Animation">
  - 애니메이션의 구성은 모두 Blueprint에서 제작했고, 점프, 속도등 관련해서 제작
  - 모든 애니메이션은 RootMotion을 사용하지 않음

- ## <span style = "color:yellow;">더블 점프</span>
  - <img src="Image/DoubleJump.gif" height="300" title="DoubleJump">
  - <img src="Image/Default_DoubleJump.png" height="300" title="Default_DoubleJump">
  - 현재 개발한 것은 제자리에서의 이중 점프에 한정
  - __플레이어__
    - 더블 점프는 단순히 JumpMaxCount의 수를 변경하면 단순히 점프의 개수를 정할 수 있음
    - 또한 공중제어를 불가능하게 하고, 점프 높이 또한 변경
  - __월드__
    - 월드의 중력을 "-3800"으로 변경하여, 나루토 게임과 유사하게 제작
  - __문제점__
    - 점프는 방향키의 입력에 따라 이동하도록 해야 함
      - 즉, 이동속도에 따른 점프 거리가 정해지는것이 아닌, 방향키 입력에 따른 일정한 거리의 이동이 이루어져야함

    <details><summary>C++ File</summary> 

    ```c++
    ANPlayer::ANPlayer() {
      ...
      /* Jump Setting */
      JumpMaxCount = 2;
      GetCharacterMovement()->JumpZVelocity = 1400.f;
      GetCharacterMovement()->AirControl = 0.f;
    }
    ```
    </details>

## **Day_3**
> **<h3>Today Dev Story</h3>**
- ## <span style = "color:yellow;">더블 점프_2</span>
  - <img src="Image/DoubleJump_2.gif" height="300" title="DoubleJump_2">
  - 방향키가 입력되어 있다면 방향으로의 점프를 하기 위해서 LaunchCharacter()메서드를 사용하였고, 현재 눌려있는 방향키를 표시하기 위해서 Enum 클래스 사용. 
    - 이는 현재 굳이 필요없지만 추후 공격의 콤보에도 사용하기에 미리 제작
  - 서버와 클라이언트 모두 LaunchCharacter()메서드를 처리하기 위해서 Server 함수 생성하여 해결

    <details><summary>C++ File</summary> 

    ```c++
    ANPlayer::ANPlayer() {
      ...
    	/* Jump Setting */
	    GetCharacterMovement()->JumpZVelocity = 1400.f;

      /* Impressed Keys */
      SetKeyUpDown(EKeyUpDown::EKUD_Default);
      SetKeyLeftRight(EKeyLeftRight::EKLR_Default);
    }
    void ANPlayer::MoveForward(float Value) {
      FRotator Rot = FRotator(0.f, GetControlRotation().Yaw, 0.f);
      AddMovementInput(UKismetMathLibrary::GetForwardVector(Rot), Value);

      if (Value > 0) SetKeyUpDown(EKeyUpDown::EKUD_Up);
      else if (Value < 0) SetKeyUpDown(EKeyUpDown::EKUD_Down);
      else SetKeyUpDown(EKeyUpDown::EKUD_Default);
    }
    void ANPlayer::MoveRight(float Value) {
      FRotator Rot = FRotator(0.f, GetControlRotation().Yaw, 0.f);
      AddMovementInput(UKismetMathLibrary::GetRightVector(Rot), Value);

      if (Value > 0) SetKeyLeftRight(EKeyLeftRight::EKLR_Right);
      else if (Value < 0) SetKeyLeftRight(EKeyLeftRight::EKLR_Left);
      else SetKeyLeftRight(EKeyLeftRight::EKLR_Default);
    }
    void ANPlayer::Jump() {
      if(!HasAuthority()) {
        ServerJump();
        return;
      }

      if (JumpCurrentCount < 2) {
        FVector ForceVec = (GetActorUpVector() * (JumpMovementForce - GetVelocity().Size())) + (GetActorForwardVector() * (JumpMovementForce - GetVelocity().Size()));
        ForceVec.Z = 1400.f;
        LaunchCharacter(ForceVec, false, false);
        JumpCurrentCount++;
      }
    }
    void ANPlayer::ServerJump_Implementation() {
      Jump();
    }
    bool ANPlayer::ServerJump_Validate() {
      return true;
    }
    ```
    </details>
    <details><summary>Header File</summary> 

    ```c++
    UENUM(BlueprintType)
    enum class EKeyUpDown : uint8 {
      EKUD_Up				UMETA(DisplayName = "Up"),
      EKUD_Down			UMETA(DisplayName = "Down"),

      EKUD_Default		UMETA(DisplayName = "Default")
    };

    UENUM(BlueprintType)
    enum class EKeyLeftRight : uint8 {
      EKLR_Right			UMETA(DisplayName = "Right"),	
      EKLR_Left			UMETA(DisplayName = "Left"),
      
      EKLR_Default		UMETA(DisplayName = "Default")
    };
    protected:
      /* Inpressed Keys */
      UPROPERTY(VisibleAnywhere, Category="Movement")
      EKeyUpDown Key_UD;

      UPROPERTY(VisibleAnywhere, Category = "Movement")
      EKeyLeftRight Key_LR;

      UPROPERTY(EditDefaultsOnly, Category = "Movement|Jump")
      float JumpMovementForce;

    	/* Jump */
      UFUNCTION(Server, Reliable, WithValidation)
      void ServerJump();
      
      FORCEINLINE void SetKeyUpDown(EKeyUpDown newKey) { Key_UD = newKey; }
      FORCEINLINE void SetKeyLeftRight(EKeyLeftRight newKey) { Key_LR = newKey; }

      FORCEINLINE EKeyUpDown GetKeyUpDown() { return Key_UD; }
      FORCEINLINE EKeyLeftRight GetKeyLeftRight() { return Key_LR; }
    public:
      virtual void Jump() override;
    ```
    </details>
    
- ## <span style = "color:yellow;">잡다한것</span>
  - 제자리 점프와 달려가면서 하는 점프간의 속도 차이가 발생
    - 기존 점프 Force에서 현재 이동 속도를 빼서 계산한 만큼 점프
      ```cpp
      ForceVec = FVector(0, -(JumpMovementForce - GetVelocity().Size()), 1400.f);
      ```
  - 기존에는 카메라가 움직이지 않았지만, 멀티에서는 움직이기 때문에 점프 방향의 문제 발생
    - 방향을 __"GetActorUpVector() + GetActorForwardVector()"로__ 처리
      ```cpp
      FVector ForceVec = (GetActorUpVector() * (JumpMovementForce - GetVelocity().Size())) + (GetActorForwardVector() * (JumpMovementForce - GetVelocity().Size()));
      ```

**<h3>Realization</h3>**
  - 제자리 점프와 달려가면서 하는 점프간의 속도 차이가 발생
    - 기존에는 움직임을 멈추고, Jump(), 하지만 이 방법은 너무 개발자답지 못한 방식이였기 때문에 기존 점프 Force에서 현재 이동 속도를 빼서 계산한 만큼 점프하도록 변경

  - 기존에는 카메라가 움직이지 않았지만, 멀티에서는 움직이기 때문에 점프 방향의 문제가 발생
    - 방향을 __"GetActorUpVector() + GetActorForwardVector()"로__ 처리

  - <img src="Image/DoubleJump_Error(Three).gif" height="300" title="DoubleJump_Error(Three)">
  - __개선점__ : 클라이언트는 점프를 3번하는 문제와 애니메이션이 실행되지 않는 문제

## **Day_4**
> **<h3>Today Dev Story</h3>**
- ## <span style = "color:yellow;">무기에 따른 애니메이션</span>
  - <img src="Image/SpawnWeapon.gif" height="300" title="SpawnWeapon">
    - 추가로 weapon에 따라 애니메이션이 달라지는 "KindOfWeapon"제작
    - 이를 위해서 Weapon클래스를 제작해야하고, Player에 장착된 Weapon의 종류에 따라 애니메이션을 다르게 처리해야한다.
      - 아직 애니메이션 처리를 하지 않았음
    - NWeapon클래스를 만들고 이를 NPlayer에서 호출하여 생성. 이때 무기는 랜덤으로 지정 (SetWeaponRandom())
      - 또한 서버에서 공유되어야 하기때문에 Replicates설정
    - NPlayer에서의 Weapon 생성은 BeginPlay()에서 진행되며 서버일때만 생성되도록 설정

    <details><summary>C++ File</summary> 

    ```c++
    //NWeapon.cpp
    ANWeapon::ANWeapon(){
      MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
      SetRootComponent(MeshComp);
      if(WeaponMeshType.Num() >= 2) MeshComp->SetStaticMesh(WeaponMeshType[0]);

      SetReplicates(true);
    }
    void ANWeapon::BeginPlay(){
      Super::BeginPlay();

      SetWeaponRandom();
    }
    void ANWeapon::SetWeaponRandom() {
      if (WeaponMeshType.Num() >= 2) {
        int32 WeaponTmp = FMath::RandRange(0, 1);
        if (WeaponTmp == 0) {
          MeshComp->SetStaticMesh(WeaponMeshType[WeaponTmp]);
          WeaponType = EWeaponType::EWT_Sword;
        }
        else {
          MeshComp->SetStaticMesh(WeaponMeshType[WeaponTmp]);
          WeaponType = EWeaponType::EWT_Blade;
        }
    }
    ```
    ```cpp
    //NPlayer.cpp
    ANPlayer::ANPlayer() {
      ...

    	WeaponAttachSocketName = "WeaponSocket";
    }
    void ANPlayer::BeginPlay() {
      ...

      /** Spawn Weapon */
      if (HasAuthority()) {
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        if (StarterWeaponClass) CurrentWeapon = GetWorld()->SpawnActor<ANWeapon>(StarterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
        if (CurrentWeapon) {
          CurrentWeapon->SetOwner(this);
          CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
        }
      }
    }
    ```
    </details>

    <details><summary>Header File</summary> 

    ```c++
    //NWeapon.h
    protected:
      UPROPERTY(EditDefaultsOnly,Category = "Weapon")
      EWeaponType WeaponType;

      /* Please Set Weapons Mesh */
      UPROPERTY(EditDefaultsOnly, Category = "Weapon")
      TArray<UStaticMesh*> WeaponMeshType;

      UPROPERTY(EditDefaultsOnly,Category = "Weapon")
      UStaticMeshComponent* MeshComp;

      /* Set Player's Weapon for Random */
      UFUNCTION()
      void SetWeaponRandom();
    public:	
      FORCEINLINE EWeaponType GetWeaponType() { return WeaponType; }
    ```
    ```c++
    //NPlayer.h
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TSubclassOf<AActor> StarterWeaponClass;

    UPROPERTY(VisibleAnywhere, Category = "Weapon")
    class ANWeapon* CurrentWeapon;

    UPROPERTY(VisibleDefaultsOnly, Category = "Weapon")
    FName WeaponAttachSocketName;
    ```
    </details>

- ## <span style = "color:yellow;">끄적 끄적</span>
  왜 안될까? 분명 클라에서만 호출해 주면되는거 아닌가? 걍 서버에서만 호출해 버릴까? 어때 괜찮지 않아? 

**<h3>Realization</h3>**
  - 무기 클래스는 생성했으나 무기에 따른 애니메이션은 생성 X 