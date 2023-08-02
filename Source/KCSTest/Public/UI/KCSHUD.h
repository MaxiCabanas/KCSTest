// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "KCSHUD.generated.h"

UCLASS()
class KCSTEST_API AKCSHUD : public AHUD
{
	GENERATED_BODY()

public:
	AKCSHUD();

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "KCS|Widgets")
	TArray<TSubclassOf<UUserWidget>> InitialWidgetsClass;

	UPROPERTY(Transient)
	UUserWidget* InitialWidgets;
};
