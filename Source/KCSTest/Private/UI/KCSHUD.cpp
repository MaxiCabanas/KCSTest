// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/KCSHUD.h"

#include "Blueprint/UserWidget.h"
#include "Types/KCSGameplayTags.h"


AKCSHUD::AKCSHUD()
{
	
}

void AKCSHUD::SwitchWidgetContainer(const FGameplayTag& GameState)
{
	ActiveWidgetContainer->RemoveFromParent();
	
	if (const TSubclassOf<UUserWidget>* WidgetContainerClass = WidgetContainers.Find(GameState))
	{
		ActiveWidgetContainer = CreateWidget<UUserWidget>(PlayerOwner, *WidgetContainerClass);
		ActiveWidgetContainer->AddToViewport();
	}
}

void AKCSHUD::BeginPlay()
{
	Super::BeginPlay();

	if (const TSubclassOf<UUserWidget>* PlayingStateWidgetClass = WidgetContainers.Find(TAG_GameState_Playing))
	{
		ActiveWidgetContainer = CreateWidget<UUserWidget>(PlayerOwner, *PlayingStateWidgetClass);
		ActiveWidgetContainer->AddToViewport();
	}
}

