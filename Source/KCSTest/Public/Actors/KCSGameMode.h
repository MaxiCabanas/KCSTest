// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "KCSGameMode.generated.h"

UCLASS()
class KCSTEST_API AKCSGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AKCSGameMode();

protected:
	virtual void BeginPlay() override;
};
