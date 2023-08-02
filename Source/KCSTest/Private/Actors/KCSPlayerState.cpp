// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/KCSPlayerState.h"


void AKCSPlayerState::AddCrystals(int32 CrystalsToAdd)
{
	ensure(CrystalsToAdd > 0);
	Crystals += CrystalsToAdd;
	GEngine->AddOnScreenDebugMessage(0, 5.0f, FColor::Green, FString::Printf(TEXT("Crystal picked!: %d"), Crystals));
}

void AKCSPlayerState::RemoveCrystals(int32 CrystalsToRemove)
{
	Crystals -= FMath::Max(CrystalsToRemove, 0);
	GEngine->AddOnScreenDebugMessage(0, 5.0f, FColor::Red, FString::Printf(TEXT("%d Crystals consumed!: new total: %d"), CrystalsToRemove, Crystals));
}

