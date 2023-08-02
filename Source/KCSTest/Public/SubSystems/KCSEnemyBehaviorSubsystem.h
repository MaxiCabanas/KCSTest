// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// KCS includes
#include "Actors/KCSWorldSettings.h"

// UE Includes
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "KCSEnemyBehaviorSubsystem.generated.h"

class AKCSEnemyPawn;
struct FKCSEnemiesRow;

USTRUCT()
struct FKCSEnemiesRow
{
	GENERATED_BODY()

public:
	FKCSEnemiesRow() {};

	FKCSEnemiesRow(int32 AmountOfEnemies)
	{
		EnemiesInRow.Reserve(AmountOfEnemies);
	}

	bool IsEmpty() const
	{
		if (EnemiesInRow.IsEmpty())
		{
			return true;
		}

		for (const TObjectPtr<AKCSEnemyPawn>& EnemyPawn : EnemiesInRow)
		{
			if (EnemyPawn)
			{
				return false;
			}
		}

		return true;
	}
	
	UPROPERTY()
	TArray<TObjectPtr<AKCSEnemyPawn>> EnemiesInRow;
};

/**
 * 
 */
UCLASS()
class KCSTEST_API UKCSEnemyBehaviorSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override {return false;};
	
	UFUNCTION(BlueprintPure, Category = "KCS|Subsystems", meta = (WorldContext = "WorldContextObject"))
	static UKCSEnemyBehaviorSubsystem* Get(const UObject* WorldContextObject);
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	// ~Begin FTickableGameObject Interface
	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(UKCSEnemyMovementSubsystem, STATGROUP_Tickables);
	}
	
	virtual UWorld* GetTickableGameObjectWorld() const override	{ return GetWorld(); }

	virtual ETickableTickType GetTickableTickType() const override { return ETickableTickType::Conditional;	}

	/** Wait we are ready to tick. */
	virtual bool IsTickable() const override { return !bIsPaused && !AllEnemies.IsEmpty(); }
	
	virtual void Tick(float DeltaTime) override;
	// ~End FTickableGameObject Interface

	void RegisterEnemies(const TArray<FKCSEnemiesRow>& Enemies);

	void GetAllEnemiesActors(TSet<TObjectPtr<AActor>>& OutEnemiesSet);

	UFUNCTION(BlueprintCallable)
	void GetNotifyEnemyReachGameBorder();

	UFUNCTION(BlueprintCallable)
	void SetIsPaused(bool bInPause) { bIsPaused = bInPause; }

protected:

	UFUNCTION()
	void OnEnemyKilled(AKCSEnemyPawn* KilledEnemyPawn);

	UFUNCTION()
	void OnEnemiesTimeDilationChanged(float TimeDilation);
	
	void UpdateEnemiesInFrontline(AKCSEnemyPawn* KilledEnemyPawn);

	void SanitizeEnemies();
	
	FVector CalculateEnemiesDirection();
	
private:

	bool bIsPaused = false;

	UPROPERTY()
	TArray<FKCSEnemiesRow> AllEnemies;

	UPROPERTY()
	TArray<TObjectPtr<AKCSEnemyPawn>> EnemiesInFrontline;

	int32 RowToMove = 0;

	FVector HorizontalDirection = FVector(-1.0f, 0.0f, 0.0f);

	bool bBorderReached = false;

	int32 RowsMovedDown = 0;
	
	FKCSEnemiesBehaviorSettings EnemiesSettings;
	
	float ElapseEnemiesMovement = 0.0f;

	float ElapseEnemiesShotRate = 0.0f;

	float EnemiesTimeDilation = 1.0f;
};
