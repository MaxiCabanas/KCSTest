// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/HUD.h"
#include "KCSHUD.generated.h"

UCLASS()
class KCSTEST_API AKCSHUD : public AHUD
{
	GENERATED_BODY()

public:
	AKCSHUD();

	void SwitchWidgetContainer(const FGameplayTag& GameState);

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "KCS|Widgets", meta = (ForceInlineRow))
	TMap<FGameplayTag, TSubclassOf<UUserWidget>> WidgetContainers;

	UPROPERTY(Transient)
	UUserWidget* ActiveWidgetContainer;
};
