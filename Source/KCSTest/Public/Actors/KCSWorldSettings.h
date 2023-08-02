// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/KCSEnemyPawn.h"
#include "GameFramework/WorldSettings.h"
#include "KCSWorldSettings.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnemiesTimeDilationEvent, float, TimeDilation);

USTRUCT()
struct FKCSEnemiesBehaviorSettings
{
	GENERATED_BODY()

public:
	FKCSEnemiesBehaviorSettings() {};

	FKCSEnemiesBehaviorSettings(float InMoveSpeedRate, float InMoveStepDistance, float InInitialAttackDelay, float InFireRate) :
	MoveSpeedRate(InMoveSpeedRate), MoveStepDistance(InMoveStepDistance), InitialAttackDelay(InInitialAttackDelay), FireRate(InFireRate) {};

	/** How often the enemies will move at the start of the game. */
	UPROPERTY(EditDefaultsOnly, meta = (Units = Seconds))
	float MoveSpeedRate = 0.5f;

	/** Distance that enemies will move when they move. */
	UPROPERTY(EditDefaultsOnly)
	float MoveStepDistance = 100.0f;

	/** Enemies will only shoot after this initial delay. */
	UPROPERTY(EditDefaultsOnly, meta = (Units = Seconds))
	float InitialAttackDelay = 2.5f;

	/** How often the enemies will shot. */
	UPROPERTY(EditDefaultsOnly, meta = (Units = Seconds))
	float FireRate = 0.75f; 
};


UCLASS()
class KCSTEST_API AKCSWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static AKCSWorldSettings* GetKCSWorldSettings(UObject* WorldContextObject);

	const FKCSEnemiesBehaviorSettings& GetEnemiesSettings() const { return EnemiesSettings; }

	int32 GetScoreForEnemy(UClass* EnemyClass);

	int32 GetInitialPlayerLives() const { return InitialPlayerLives; }
	
	UPROPERTY(BlueprintAssignable)
	FEnemiesTimeDilationEvent OnEnemiesTimeDilationChanged;

protected:
	
	UPROPERTY(Config, EditDefaultsOnly, Category = "KCS|Enemy", meta = (ShowOnlyInnerProperties))
	FKCSEnemiesBehaviorSettings EnemiesSettings;

	UPROPERTY(Config, EditDefaultsOnly, Category = "KCS|Enemy")
	TMap<TSubclassOf<AKCSEnemyPawn>, int32> EnemyScore;

	UPROPERTY(Config, EditDefaultsOnly, Category = "KCS|Player")
	int32 InitialPlayerLives = 3;
};
