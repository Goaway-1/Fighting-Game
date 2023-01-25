#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NPlayerController.generated.h"

UENUM(BlueprintType)
enum class EWidgetState : uint8 {
	EWS_Health				UMETA(DisplayName = "Health"),
	EWS_Chacra				UMETA(DisplayName = "Chacra"),
	EWS_Switch				UMETA(DisplayName = "Switch"),
	EWS_Score				UMETA(DisplayName = "Score"),
	EWS_Condition			UMETA(DisplayName = "Condition")
};

UCLASS()
class NARUTO_API ANPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ANPlayerController();
protected:
	virtual void BeginPlay() override;
	virtual void TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;

public:
	UPROPERTY()
	class ANPlayer* OwnerPlayer;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
	class UUserWidget* MainWidget;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
	class UCutSceneWidget* CutSceneWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<class UUserWidget> MainWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<class UUserWidget> CutSceneWidgetClass;
	
protected:
	UPROPERTY()
	class UAnimMontage* EndMongtage;

public:
	UFUNCTION()
	void PlayCutScene(UMediaSource* Source, UAnimMontage* Mongtage,float MediaLength);		// Play Cut Scene in Widget : IN LOCAL
	
	UFUNCTION()
	void EndCutScene();																		// Play  End Montage each Player's

#pragma region WIDGET & GAMEMODE
private:
	TArray<class UPlayerStateWidget*> HealthWidgets;
	TArray<class UPlayerStateWidget*> ChacraWidgets;
	TArray<class UPlayerStateWidget*> SideStepWidgets;
	TArray<class UPlayerStateWidget*> ScoreWidgets;
	class UTextBlock* MiddleScreenText;
	class UTextBlock* RoundTimerText;

	class ANGameMode* CurGameMode;
	bool bIsRoundStart;

	UFUNCTION(Client, Reliable)
	void UpdateWidget(int idx, EWidgetState State, float value);			// Update Widget Only Client
		
	UFUNCTION(Client, Reliable)
	void ResetWidget();														// INIT Widget

	// Start Round..
	// time : GameMode Time, text : GameMode State, bisForce : custom...
	UFUNCTION(Client, Reliable)
	void SetRoundInfo(int time, const FString& text, bool bIsForce = false);		

public:
	UFUNCTION(Server, Reliable, WithValidation)
	void SetWidget(const EWidgetState State);								// Some Player State Changed..

	FORCEINLINE bool GetIsRoundStart() {return bIsRoundStart;}

	UFUNCTION()
	void RequestRoundEnd();													// Request Restart Round..

	UFUNCTION()
	void RequestGameOver();													// Request Game Over.
#pragma endregion


};
