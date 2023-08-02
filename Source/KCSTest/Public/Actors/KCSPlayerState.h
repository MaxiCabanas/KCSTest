// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "KCSPlayerState.generated.h"

UCLASS()
class KCSTEST_API AKCSPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	int32 GetCrystalsQuantity() const { return Crystals; }

	UFUNCTION(BlueprintCallable)
	void AddCrystals(int32 CrystalsToAdd);

	void RemoveCrystals(int32 CrystalsToRemove);

private:

	int32 Crystals = 0;
};
