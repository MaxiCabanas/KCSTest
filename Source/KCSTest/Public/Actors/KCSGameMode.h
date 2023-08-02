// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/KCSEnemyPawn.h"
#include "GameFramework/GameModeBase.h"
#include "KCSGameMode.generated.h"

class AKCSPlayerController;
class AKCSPlayerPawn;

UCLASS()
class KCSTEST_API AKCSGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AKCSGameMode();
	
	virtual void InitGameState() override;

protected:
	
	virtual void FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation) override;

	UFUNCTION()
	void OnPlayerPawnKilled();
	
	UFUNCTION()
	void OnGameStateReady(AKCSGameState* KCSGameState);

	UFUNCTION()
	void OnEnemyKilled(AKCSEnemyPawn* EnemyPawn);
	
	virtual void BeginPlay() override;

private:

	UPROPERTY(Transient)
	TObjectPtr<AKCSPlayerController> PlayerController;
};
