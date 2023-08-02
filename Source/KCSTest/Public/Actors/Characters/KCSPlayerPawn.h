// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// KCS includes
#include "KCSPawn.h"

// UE Includes
#include "CoreMinimal.h"
#include "KCSPlayerPawn.generated.h"

UCLASS()
class KCSTEST_API AKCSPlayerPawn : public AKCSPawn
{
	GENERATED_BODY()

public:
	AKCSPlayerPawn();

protected:

	virtual void PawnKilled() override;
};
