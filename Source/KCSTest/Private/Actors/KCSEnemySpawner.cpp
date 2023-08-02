// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/KCSEnemySpawner.h"

#include "Actors/KCSGameState.h"
#include "SubSystems/KCSEnemyBehaviorSubsystem.h"


AKCSEnemySpawner::AKCSEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AKCSEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	if (Enemies.IsEmpty())
	{
		SpawnEnemies();
	}
	//UKCSEnemyBehaviorSubsystem::Get(GetWorld())->RegisterEnemies(Enemies);

	AKCSGameState* GameState = GetWorld()->GetGameState<AKCSGameState>();
	GameState->RegisterEnemies(Enemies);
}

void AKCSEnemySpawner::SpawnEnemies()
{
	DestroyAllEnemies();
	
	EnemiesToSpawn.Sort();

	const FVector& InitialLocation = GetActorLocation();
	FVector SpawnLocation = GetActorLocation();
	int32 CurrentRow = 0;

	for (const FKCSEnemyParameters& EnemyParameters : EnemiesToSpawn)
	{
		for (int i = 0; i < EnemyParameters.NumberOfRows; ++i)
		{
			Enemies.Add(FKCSEnemiesRow(EnemiesPerRow));
			
			SpawnLocation.Y = InitialLocation.Y + (VerticalOffset * CurrentRow);
			
			for (int j = 0; j < EnemiesPerRow; ++j)
			{
				SpawnLocation.X = InitialLocation.X + (HorizontalOffset * j);

				FActorSpawnParameters SpawnParameters;
				SpawnParameters.Owner = this;

				AKCSEnemyPawn* NewEnemyInstance = GetWorld()->SpawnActor<AKCSEnemyPawn>(EnemyParameters.EnemyClass, SpawnLocation, InitialRotation, SpawnParameters);
				NewEnemyInstance->SetIndex(FIntVector2(CurrentRow, j));
				NewEnemyInstance->SetFolderPath("Enemies");
				Enemies[CurrentRow].EnemiesInRow.Add(NewEnemyInstance);
			}
			
			CurrentRow++;
		}
	}
}

void AKCSEnemySpawner::DestroyAllEnemies()
{
	for (FKCSEnemiesRow& EnemiesRow : Enemies)
	{
		for (AActor* Enemy : EnemiesRow.EnemiesInRow)
		{
			if (Enemy)
			{
				Enemy->Destroy();
			}
		}
	}
	Enemies.Empty();
}