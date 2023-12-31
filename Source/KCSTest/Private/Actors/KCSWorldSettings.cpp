﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/KCSWorldSettings.h"

AKCSWorldSettings* AKCSWorldSettings::GetKCSWorldSettings(UObject* WorldContextObject)
{
	if (const UWorld* World = WorldContextObject->GetWorld())
	{
		return CastChecked<AKCSWorldSettings>(World->GetWorldSettings());
	}
	return nullptr;
}

int32 AKCSWorldSettings::GetScoreForEnemy(UClass* EnemyClass)
{
	const int* Score = EnemyScore.Find(EnemyClass);
	return *Score;
}
