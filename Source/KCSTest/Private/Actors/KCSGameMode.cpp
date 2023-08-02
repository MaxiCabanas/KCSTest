// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/KCSGameMode.h"

#include "Actors/KCSGameState.h"
#include "Actors/KCSPlayerController.h"
#include "Actors/KCSPlayerState.h"
#include "Actors/Characters/KCSEnemyPawn.h"
#include "Actors/Characters/KCSPlayerPawn.h"
#include "Engine/PawnIterator.h"


AKCSGameMode::AKCSGameMode()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AKCSGameMode::InitGameState()
{
	Super::InitGameState();

	CastChecked<AKCSGameState>(GameState)->OnGameStateReady.AddUniqueDynamic(this, &ThisClass::OnGameStateReady);
}

void AKCSGameMode::FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation)
{
	Super::FinishRestartPlayer(NewPlayer, StartRotation);

	PlayerController = CastChecked<AKCSPlayerController>(NewPlayer);
	PlayerController->OnPawnDestroyed.AddUniqueDynamic(this, &ThisClass::AKCSGameMode::OnPlayerPawnKilled);
}

void AKCSGameMode::OnPlayerPawnKilled()
{
	const AKCSPlayerState* PlayerState = PlayerController->GetPlayerState<AKCSPlayerState>();

	if (PlayerState->HasAnyLiveLeft())
	{
		const UWorld* World = GetWorld();
		// AWorldSettings* WorldSettings = World->GetWorldSettings();
		//WorldSettings->SetTimeDilation(0.0f);

		FTimerHandle TimerHandle;
		FTimerDelegate TimerDelegate;

		TimerDelegate.BindWeakLambda(this, [&]()
		{
			RestartPlayer(PlayerController);
		});

		World->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 5.0f, false);
	}
	
}

void AKCSGameMode::OnGameStateReady(AKCSGameState* KCSGameState)
{
	TSet<TObjectPtr<AKCSEnemyPawn>> AllEnemies;

	KCSGameState->GetAllEnemies(AllEnemies);

	for (const TObjectPtr<AKCSEnemyPawn>& Enemy : AllEnemies)
	{
		Enemy->OnEnemyKilled.AddUniqueDynamic(this, &ThisClass::OnEnemyKilled);
	}
}

void AKCSGameMode::OnEnemyKilled(AKCSEnemyPawn* EnemyPawn)
{
	const UWorld* World = GetWorld();
	AKCSWorldSettings* WorldSettings = CastChecked<AKCSWorldSettings>(World->GetWorldSettings());
	const int32 ScoreForEnemy = WorldSettings->GetScoreForEnemy(EnemyPawn->GetClass());
	
	AKCSPlayerState::AddScore(this, ScoreForEnemy);
}

void AKCSGameMode::BeginPlay()
{
	Super::BeginPlay();
	
}
