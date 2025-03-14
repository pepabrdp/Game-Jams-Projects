// Fill out your copyright notice in the Description page of Project Settings.


#include "ControlsMenuWidget.h"



void UControlsMenuWidget::GoBack()  
{
    if (PreviousWidget)
    {
        PreviousWidget->SetVisibility(ESlateVisibility::Visible);
        RemoveFromParent();
    }
}