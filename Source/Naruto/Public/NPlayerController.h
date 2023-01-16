// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NPlayerController.generated.h"

UENUM(BlueprintType)
enum class EWidgetState : uint8 {
	EWS_Health			UMETA(DisplayName = "Health"),
	EWS_Chacra			UMETA(DisplayName = "Chacra"),
	EWS_Switch			UMETA(DisplayName = "Switch")
};

UCLASS()
class NARUTO_API ANPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ANPlayerController();
protected:
	virtual void BeginPlay() override;

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

#pragma region HEALTH&CHACRA
public:
	UPROPERTY(VisibleAnywhere, Category = "Widget")
	TArray<class UPlayerStateWidget*> HealthWidgets;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
	TArray<class UPlayerStateWidget*> ChacraWidgets;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
	TArray<class UPlayerStateWidget*> SideStepWidgets;
#pragma endregion

#pragma region WIDGET
private:
	UFUNCTION(Client, Reliable)
	void UpdateWidget(int idx, EWidgetState State, float value);			// Update Widget Only Client
		
	UFUNCTION(Client, Reliable)
	void ResetWidget();														// INIT Widget
public:
	UFUNCTION(Server, Reliable, WithValidation)
	void SetWidget(const EWidgetState State);								// Some Player State Changed..
#pragma endregion
};
