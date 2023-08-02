// Fill out your copyright notice in the Description page of Project Settings.

// KCS includes
#include "Actors/Characters/KCSPlayerPawn.h"

// UE Includes
#include "Actors/KCSPlayerState.h"

AKCSPlayerPawn::AKCSPlayerPawn()
{
	ShotDirection = FVector(0,1,0);
}

void AKCSPlayerPawn::PawnKilled()
{
	Super::PawnKilled();

	AKCSPlayerState* KCSPlayerState = CastChecked<AKCSPlayerState>(GetPlayerState());
	KCSPlayerState->OnLiveLost();
}
