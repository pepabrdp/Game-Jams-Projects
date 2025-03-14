// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ControlsMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class DEVILSMAYHEM_API UControlsMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category="Menu")
	UUserWidget* PreviousWidget;

	UFUNCTION(BlueprintCallable, Category="Menu")
	void GoBack();	
};
