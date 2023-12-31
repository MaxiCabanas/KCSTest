﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// KCS includes
#include "Interfaces/KCSTimelineOwnerInterface.h"
#include "KCSWorldSettings.h"

// UE Includes
#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "KCSGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FKCSGameStateFloatEvent, float, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FKCSGameStateEvent, class AKCSGameState*, GameState);

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

UCLASS()
class KCSTEST_API AKCSGameState : public AGameStateBase, public IKCSTimelineOwnerInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AKCSGameState();

	// ~Begin IKCSTimelineOwnerInterface
	virtual UKCSTimelineComponent* GetTimelineComponent() override { return TimelineComponent; }
	// ~End IKCSTimelineOwnerInterface
	
	virtual void BeginPlay() override;

	void SetGameStateTickEnabled(bool bEnabled);

	void RegisterEnemies(const TArray<FKCSEnemiesRow>& Enemies);

	void GetAllEnemies(TSet<TObjectPtr<AKCSEnemyPawn>>& OutEnemiesSet);

	void GetAllTimelineOwners(TSet<TScriptInterface<IKCSTimelineOwnerInterface>>& TimelineOwners);

	UFUNCTION(BlueprintCallable)
	void GetNotifyEnemyReachGameBorder();

	void SetGameStateTimeDilation(float NewTimeDilation);

	int32 GetEnemiesAliveCount() const;

	virtual void TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;

	UPROPERTY(BlueprintAssignable)
	FKCSGameStateFloatEvent OnGameStateTimeDilationChanged;

	UPROPERTY(BlueprintAssignable)
	FKCSGameStateEvent OnGameStateReady;
	
protected:

	UFUNCTION()
	void OnEnemyKilled(AKCSEnemyPawn* KilledEnemyPawn);

	void UpdateEnemiesInFrontline(AKCSEnemyPawn* KilledEnemyPawn);

	void SanitizeEnemies();
	
	FVector CalculateEnemiesDirection();

	/** This curve describes how the speed of the enemies will increase while they die. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "KCS|Enemies")
	FRuntimeFloatCurve EnemiesSpeedMultiplier;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UKCSTimelineComponent* TimelineComponent;
	
private:
	
	// All this variables will be recorded, so they need to be floats.
	UPROPERTY(Transient)
	float RowToMove = 0.0f;
	
	UPROPERTY(Transient)
	float BorderReached = 0.0f;
	
	UPROPERTY(Transient)
	float RowsMovedDown = 0.0f;
	
	UPROPERTY(Transient)
	float HorizontalDirection = 1.0f;
	// end recordable variables

	UPROPERTY(Transient)
	TArray<FKCSEnemiesRow> AllEnemies;

	UPROPERTY(Transient)
	TArray<TObjectPtr<AKCSEnemyPawn>> EnemiesInFrontline;
	
	UPROPERTY(Transient)
	FKCSEnemiesBehaviorSettings EnemiesSettings;
	
	UPROPERTY(Transient)
	float ElapseEnemiesMovement = 0.0f;

	UPROPERTY(Transient)
	float ElapseEnemiesShotRate = 0.0f;

	UPROPERTY(Transient)
	int32 EnemiesInitialAmount = 0;
};