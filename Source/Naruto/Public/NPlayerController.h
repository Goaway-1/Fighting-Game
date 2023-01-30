#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NPlayerController.generated.h"

/** Widget State... */
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

protected:
	class ANPlayer* OwnerPlayer;
	class UUserWidget* MainWidget;

	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<class UUserWidget> MainWidgetClass;

#pragma region CUTSCENE
private:
	class UCutSceneWidget* CutSceneWidget;
	class UAnimMontage* EndMongtage;

protected:
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<class UUserWidget> CutSceneWidgetClass;
public:
	void PlayCutScene(UMediaSource* Source, UAnimMontage* Mongtage,float MediaLength);		// Play Cut Scene in Widget : IN LOCAL
	
	void EndCutScene();																		// Play  End Montage each Player's
#pragma endregion

#pragma region WIDGET
private:
	TArray<class UPlayerStateWidget*> HealthWidgets;
	TArray<class UPlayerStateWidget*> ChacraWidgets;
	TArray<class UPlayerStateWidget*> SideStepWidgets;
	TArray<class UPlayerStateWidget*> ScoreWidgets;
	class UTextBlock* MiddleScreenText;
	class UTextBlock* RoundTimerText;

	UFUNCTION(Client, Reliable)
	void UpdateWidget(int idx, EWidgetState State, float value);			// Update Widget Only Client
		
	UFUNCTION(Client, Reliable)
	void ResetWidget();														// INIT Widget
public:
	UFUNCTION(Server, Reliable, WithValidation)
	void SetWidget(const EWidgetState State);								// Set Widget Value when Some Player State Changed..
#pragma endregion

#pragma region GAMEMODE
	class ANGameMode* CurGameMode;
	bool bIsRoundStart;

	/** 
	* Set Round Info...
	* @param time			GameMode Time
	* @param text			GameMode State
	* @param bisForce		Chanage Force
	*/
	UFUNCTION(Client, Reliable)
	void SetRoundInfo(int time, const FString& text, bool bIsForce = false);	

public:
	FORCEINLINE bool GetIsRoundStart() {return bIsRoundStart;}

	void RequestRoundEnd();													// Request Restart Round..
	void RequestGameOver();													// Request Game Over.
#pragma endregion
};
