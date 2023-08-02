// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// KCS includes
#include "Characters/KCSEnemyPawn.h"

// UE Includes
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KCSEnemySpawner.generated.h"

struct FKCSEnemiesRow;

USTRUCT()
struct FKCSEnemyParameters
{
	GENERATED_BODY()

public:

	/** The class for this enemy. */
	UPROPERTY(EditAnywhere)
	TSubclassOf<AKCSEnemyPawn> EnemyClass;

	/** The number of rows that will spawn for this enemy. */
	UPROPERTY(EditAnywhere)
	uint8 NumberOfRows = 1;

	/** The height this enemy will spawn in relation with the other enemies
	 * i.e: Enemy rows with height = 1 will spawn above rows with height = 0 and below rows with height = 2. */
	UPROPERTY(EditAnywhere)
	uint8 Height = 0;

	friend bool operator<(const FKCSEnemyParameters& Lhs, const FKCSEnemyParameters& Rhs)
	{
		return Lhs.Height < Rhs.Height;
	}
};

UCLASS()
class KCSTEST_API AKCSEnemySpawner : public AInfo
{
	GENERATED_BODY()

public:
	AKCSEnemySpawner();

	virtual void BeginPlay() override;

protected:

	UFUNCTION(CallInEditor, Category = "KCS|Editor")
	void SpawnEnemies();

	UPROPERTY(EditAnywhere, Category = "KCS|Enemies")
	TArray<FKCSEnemyParameters> EnemiesToSpawn;

	UPROPERTY(EditAnywhere, Category = "KCS|Enemies")
	float HorizontalOffset = 150.0f;

	UPROPERTY(EditAnywhere, Category = "KCS|Enemies")
	float VerticalOffset = 200.0f;

	UPROPERTY(EditAnywhere, Category = "KCS|Enemies")
	FRotator InitialRotation = FRotator(0.0f, -90.0f, 0.0f);

	UPROPERTY(EditAnywhere, Category = "KCS|Enemies")
	uint8 EnemiesPerRow = 11;

private:

	void DestroyAllEnemies();
	
	UPROPERTY()
	TArray<FKCSEnemiesRow> Enemies;
};
