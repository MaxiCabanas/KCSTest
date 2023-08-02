// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/KCSHUD.h"

#include "Blueprint/UserWidget.h"


AKCSHUD::AKCSHUD()
{
	
}

void AKCSHUD::BeginPlay()
{
	Super::BeginPlay();

	if (!InitialWidgetsClass.IsEmpty())
	{
		for (const TSubclassOf<UUserWidget>& WidgetClass : InitialWidgetsClass)
		{
			InitialWidgets = CreateWidget<UUserWidget>(PlayerOwner, WidgetClass);
			InitialWidgets->AddToViewport();
		}
	}
}

