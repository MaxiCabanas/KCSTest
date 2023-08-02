// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/KCSGameState.h"

#include "Actors/KCSGameMode.h"
#include "Actors/Characters/KCSEnemyPawn.h"

AKCSGameState::AKCSGameState()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);
	SetTickableWhenPaused(false);

	TimelineComponent = CreateDefaultSubobject<UKCSTimelineComponent>(TEXT("TimelineComponent"));
}

void AKCSGameState::BeginPlay()
{
	Super::BeginPlay();

	const UWorld* World = GetWorld();
	const AKCSWorldSettings* WorldSettings = CastChecked<AKCSWorldSettings>(World->GetWorldSettings());

	EnemiesSettings = WorldSettings->GetEnemiesSettings();
	ElapseEnemiesMovement = EnemiesSettings.MoveSpeedRate;
	ElapseEnemiesShotRate = EnemiesSettings.FireRate + EnemiesSettings.InitialAttackDelay;
}

void AKCSGameState::SetGameStateTickEnabled(bool bEnabled)
{
	SetActorTickEnabled(bEnabled);
}

void AKCSGameState::GetNotifyEnemyReachGameBorder()
{
	BorderReached = true;
}

void AKCSGameState::SetGameStateTimeDilation(float NewTimeDilation)
{
	CustomTimeDilation = NewTimeDilation;
	OnGameStateTimeDilationChanged.Broadcast(CustomTimeDilation);
}

int32 AKCSGameState::GetEnemiesAliveCount() const
{
	int32 Count = 0;
	
	for (const FKCSEnemiesRow& EnemiesRow : AllEnemies)
	{
		for (const TObjectPtr<AKCSEnemyPawn>& EnemyPawn : EnemiesRow.EnemiesInRow)
		{
			if (EnemyPawn && IsValid(EnemyPawn))
			{
				Count++;
			}
		}
	}

	return Count;
}

void AKCSGameState::RegisterEnemies(const TArray<FKCSEnemiesRow>& Enemies)
{
	AllEnemies = Enemies;

	if (AllEnemies.IsValidIndex(0))
	{
		EnemiesInFrontline = AllEnemies[0].EnemiesInRow;
	}

	EnemiesInitialAmount = 0;
	for (FKCSEnemiesRow& EnemiesRow : AllEnemies)
	{
		for (const TObjectPtr<AKCSEnemyPawn>& Enemy : EnemiesRow.EnemiesInRow)
		{
			EnemiesInitialAmount++;
			Enemy->OnEnemyKilled.AddUniqueDynamic(this, &ThisClass::OnEnemyKilled);
		}
	}

	RowsMovedDown = AllEnemies.Num();
	SetActorTickEnabled(true); 
	OnGameStateReady.Broadcast(this);
}

void AKCSGameState::GetAllEnemies(TSet<TObjectPtr<AKCSEnemyPawn>>& OutEnemiesSet)
{
	for (const FKCSEnemiesRow& EnemiesRow : AllEnemies)
	{
		for (const TObjectPtr<AKCSEnemyPawn> Enemy : EnemiesRow.EnemiesInRow)
		{
			if (Enemy)
			{
				OutEnemiesSet.Add(Enemy);
			}
		}
	}
}

void AKCSGameState::GetAllTimelineOwners(TSet<TScriptInterface<IKCSTimelineOwnerInterface>>& TimelineOwners)
{
	for (const FKCSEnemiesRow& EnemiesRow : AllEnemies)
	{
		for (const TObjectPtr<AKCSEnemyPawn> Enemy : EnemiesRow.EnemiesInRow)
		{
			if (Enemy && IsValid(Enemy))
			{
				TimelineOwners.Add(Enemy);
			}
		}
	}

	TimelineOwners.Add(this);
}

void AKCSGameState::OnEnemyKilled(AKCSEnemyPawn* KilledEnemyPawn)
{
	UpdateEnemiesInFrontline(KilledEnemyPawn);
	SanitizeEnemies();
}

void AKCSGameState::UpdateEnemiesInFrontline(AKCSEnemyPawn* KilledEnemyPawn)
{
	if (!EnemiesInFrontline.Contains(KilledEnemyPawn))
	{
		return;
	}

	const int32 EnemyIndexInFrontline = EnemiesInFrontline.Find(KilledEnemyPawn);
	FIntVector2 EnemyIndex;
	KilledEnemyPawn->GetIndex(EnemyIndex);

	for (int i = EnemyIndex.X + 1; i < AllEnemies.Num(); ++i)
	{
		const TObjectPtr<AKCSEnemyPawn>& EnemyAbove = AllEnemies[i].EnemiesInRow[EnemyIndex.Y];
		if (IsValid(EnemyAbove))
		{
			EnemiesInFrontline[EnemyIndexInFrontline] = EnemyAbove;
			return;
		}
	}

 	EnemiesInFrontline.RemoveAt(EnemyIndexInFrontline);
}

void AKCSGameState::SanitizeEnemies()
{
	// We only remove empty rows to preserve enemies indexes.
	for (int i = AllEnemies.Num(); --i >= 0;)
	{
		if (AllEnemies.IsEmpty())
		{
			AllEnemies.RemoveAt(i);
		}
	}
}

FVector AKCSGameState::CalculateEnemiesDirection()
{
	// We only move down the enemies when if the first row is about to be moved so we are sure all other reached 
	// the game border.
	if (BorderReached == 1.0f && FMath::IsNearlyZero(RowToMove,0.1f))
	{
		RowsMovedDown = 0;
		HorizontalDirection *= -1.0f;
		BorderReached = 0.0f;
	}
		
	if (RowsMovedDown < AllEnemies.Num())
	{
		RowsMovedDown += 1.0f;
		return FVector(0.0f, -1.0f, 0.0f);
	}
	
	return FVector(-1.0f, 0.0f, 0.0f) * HorizontalDirection;
}

void AKCSGameState::TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);

	//TODO GetEnemiesAliveCount() is too slow to be right calling it here, but don't have time to fix it.
	const float EnemiesAlivesScalar = GetEnemiesAliveCount() / static_cast<float>(EnemiesInitialAmount);
	ElapseEnemiesMovement -= (DeltaTime * EnemiesSpeedMultiplier.EditorCurveData.Eval(EnemiesAlivesScalar)) ;
	ElapseEnemiesShotRate -= DeltaTime;

	// TODO: I should move this and the shot logic to two different components but I don't gonna have the time.
	// Move Enemies.
	if (ElapseEnemiesMovement <= 0)
	{
		const FVector MovementDir = CalculateEnemiesDirection();
		const int32 RowToMoveAsInt = FMath::RoundToInt32(RowToMove);

		for (const TObjectPtr<AKCSEnemyPawn>& Enemy : AllEnemies[RowToMoveAsInt].EnemiesInRow)
		{
			if (Enemy)
			{
				FVector EnemyLocation = Enemy->GetActorLocation();
				EnemyLocation += (MovementDir * EnemiesSettings.MoveStepDistance);
				Enemy->TeleportTo(EnemyLocation, Enemy->GetActorRotation());
			}
		}

		RowToMove = (RowToMoveAsInt + 1) % AllEnemies.Num();
		ElapseEnemiesMovement = EnemiesSettings.MoveSpeedRate;
	}

	// Make enemies shot.
	if (ElapseEnemiesShotRate <= 0 && !EnemiesInFrontline.IsEmpty())
	{
		const int32 RandEnemyIndex = FMath::RandRange(0, EnemiesInFrontline.Num() - 1);
		if (ensure(EnemiesInFrontline[RandEnemyIndex]))
		{
			EnemiesInFrontline[RandEnemyIndex]->Shoot();
			ElapseEnemiesShotRate = EnemiesSettings.FireRate;
		}
	}
}
