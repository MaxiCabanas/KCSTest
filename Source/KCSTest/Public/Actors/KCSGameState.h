﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KCSWorldSettings.h"
#include "GameFramework/GameStateBase.h"
#include "Interfaces/KCSTimelineOwnerInterface.h"
#include "KCSGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FKCSGameStateFloatEvent, float, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FKCSGameStateBoolEvent, bool, bValue);

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

	void GetAllEnemiesActors(TSet<TObjectPtr<AActor>>& OutEnemiesSet);

	UFUNCTION(BlueprintCallable)
	void GetNotifyEnemyReachGameBorder();

	void SetGameStateTimeDilation(float NewTimeDilation);

	virtual void TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;

	UPROPERTY(BlueprintAssignable)
	FKCSGameStateFloatEvent OnGameStateTimeDilationChanged;
	
protected:

	UFUNCTION()
	void OnEnemyKilled(AKCSEnemyPawn* KilledEnemyPawn);

	void UpdateEnemiesInFrontline(AKCSEnemyPawn* KilledEnemyPawn);

	void SanitizeEnemies();
	
	FVector CalculateEnemiesDirection();
	
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
};