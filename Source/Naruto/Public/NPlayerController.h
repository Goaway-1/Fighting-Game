// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class NARUTO_API ANPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ANPlayerController();
protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, Category = "Widget")
	class UUserWidget* PlayerWidget;

	UPROPERTY(VisibleAnywhere, Category = "Widget")
	class UCutSceneWidget* CutSceneWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<class UUserWidget> MainSource;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<class UUserWidget> CutSceneSource;
	
protected:
	UPROPERTY()
	class UAnimMontage* EndMongtage;

public:
	UFUNCTION()
	void PlayCutScene(UMediaSource* Source, UAnimMontage* Mongtage,float MediaLength);		// Play Cut Scene in Widget : IN LOCAL
	
	UFUNCTION()
	void EndCutScene();																		// Play  End Montage each Player's
};
