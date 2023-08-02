// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/KCSGameMode.h"

// KCS Includes
#include "Actors/KCSGameState.h"
#include "Actors/KCSPlayerController.h"
#include "Actors/KCSPlayerState.h"
#include "Actors/Characters/KCSEnemyPawn.h"
#include "Types/KCSGameplayTags.h"
#include "UI/KCSHUD.h"


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

void AKCSGameMode::SetGameState(const FGameplayTag& InGameState)
{
	SetPause(PlayerController);
	AKCSPlayerState* PlayerState = PlayerController->GetPlayerState<AKCSPlayerState>();
	PlayerState->MatchEnded();
}

void AKCSGameMode::OnPlayerPawnKilled()
{
	const AKCSPlayerState* PlayerState = PlayerController->GetPlayerState<AKCSPlayerState>();

	if (PlayerState->HasAnyLiveLeft())
	{
		RespawnPlayerPawn();
	}
	else
	{
		SetMatchEndeed();
	}
}

void AKCSGameMode::RespawnPlayerPawn()
{
	const UWorld* World = GetWorld();

	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
		
	TimerDelegate.BindWeakLambda(this, [&]()
	{
		GetWorld()->GetGameState<AKCSGameState>()->SetGameStateTimeDilation(1.0f);
		RestartPlayer(PlayerController);
	});

	constexpr float PauseDuration = 5.0f;

	World->GetGameState<AKCSGameState>()->SetGameStateTimeDilation(0.0f);
	World->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, PauseDuration, false);
}

void AKCSGameMode::SetMatchEndeed()
{
	const FNativeGameplayTag& NewGameState = TAG_GameState_WaitingCoin;
	SetGameState(NewGameState);
	AKCSHUD* HUD = PlayerController->GetHUD<AKCSHUD>();
	HUD->SwitchWidgetContainer(NewGameState);
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
	
	const AKCSGameState* KCSGameState = CastChecked<AKCSGameState>(GameState);

	// TODO Quick fix: since we broadcast this delegate before Destroy() is called, the current destroyed enemy is included in this counter.
	// so we substract one.
	const int32 EnemiesAliveCount = KCSGameState->GetEnemiesAliveCount() - 1;

	if (EnemiesAliveCount == 0)
	{
		SetMatchEndeed();
	}
}

void AKCSGameMode::BeginPlay()
{
	Super::BeginPlay();
	
}
