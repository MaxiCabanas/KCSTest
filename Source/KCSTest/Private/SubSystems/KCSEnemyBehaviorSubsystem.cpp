// Fill out your copyright notice in the Description page of Project Settings.

#include "SubSystems/KCSEnemyBehaviorSubsystem.h"

#include "Actors/Characters/KCSEnemyPawn.h"
#include "Actors/KCSWorldSettings.h"


UKCSEnemyBehaviorSubsystem* UKCSEnemyBehaviorSubsystem::Get(const UObject* WorldContextObject)
{
	return WorldContextObject->GetWorld()->GetGameInstance()->GetSubsystem<UKCSEnemyBehaviorSubsystem>();
}

void UKCSEnemyBehaviorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	const UWorld* World = GetWorld();
	AKCSWorldSettings* WorldSettings = CastChecked<AKCSWorldSettings>(World->GetWorldSettings());

	EnemiesSettings = WorldSettings->GetEnemiesSettings();
	ElapseEnemiesMovement = EnemiesSettings.MoveSpeedRate;
	ElapseEnemiesShotRate = EnemiesSettings.FireRate + EnemiesSettings.InitialAttackDelay;
	
	//EnemiesTimeDilation = WorldSettings->GetEnemiesTimeDilation();
	WorldSettings->OnEnemiesTimeDilationChanged.AddUniqueDynamic(this, &ThisClass::OnEnemiesTimeDilationChanged);
}

void UKCSEnemyBehaviorSubsystem::GetNotifyEnemyReachGameBorder()
{
	bBorderReached = true;
}

void UKCSEnemyBehaviorSubsystem::RegisterEnemies(const TArray<FKCSEnemiesRow>& Enemies)
{
	AllEnemies = Enemies;

	if (AllEnemies.IsValidIndex(0))
	{
		EnemiesInFrontline = AllEnemies[0].EnemiesInRow;
	}

	for (FKCSEnemiesRow& EnemiesRow : AllEnemies)
	{
		for (const TObjectPtr<AKCSEnemyPawn>& Enemy : EnemiesRow.EnemiesInRow)
		{
			Enemy->OnEnemyKilled.AddUniqueDynamic(this, &ThisClass::OnEnemyKilled);
		}
	}

	RowsMovedDown = AllEnemies.Num();
}

void UKCSEnemyBehaviorSubsystem::GetAllEnemiesActors(TSet<TObjectPtr<AActor>>& OutEnemiesSet)
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

void UKCSEnemyBehaviorSubsystem::OnEnemyKilled(AKCSEnemyPawn* KilledEnemyPawn)
{
	UpdateEnemiesInFrontline(KilledEnemyPawn);
	SanitizeEnemies();
}

void UKCSEnemyBehaviorSubsystem::OnEnemiesTimeDilationChanged(float TimeDilation)
{
	EnemiesTimeDilation = TimeDilation;
}

void UKCSEnemyBehaviorSubsystem::UpdateEnemiesInFrontline(AKCSEnemyPawn* KilledEnemyPawn)
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

void UKCSEnemyBehaviorSubsystem::SanitizeEnemies()
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

FVector UKCSEnemyBehaviorSubsystem::CalculateEnemiesDirection()
{
	// We only move down the enemies when if the first row is about to be moved so we are sure all other reached 
	// the game border.
	if (bBorderReached && RowToMove == 0)
	{
		RowsMovedDown = 0;
		HorizontalDirection *= -1.0f;
		bBorderReached = false;
	}
		
	if (RowsMovedDown < AllEnemies.Num())
	{
		RowsMovedDown++;
		return FVector(0.0f, -1.0f, 0.0f);
	}
	
	return HorizontalDirection;
}

void UKCSEnemyBehaviorSubsystem::Tick(float DeltaTime)
{
	const float ActualDeltaTime = DeltaTime * EnemiesTimeDilation;
	ElapseEnemiesMovement -= ActualDeltaTime;
	ElapseEnemiesShotRate -= ActualDeltaTime;

	// Move Enemies.
	if (ElapseEnemiesMovement <= 0)
	{
		const FVector MovementDir = CalculateEnemiesDirection();

		for (const TObjectPtr<AKCSEnemyPawn>& Enemy : AllEnemies[RowToMove].EnemiesInRow)
		{
			if (Enemy)
			{
				FVector EnemyLocation = Enemy->GetActorLocation();
				EnemyLocation += (MovementDir * EnemiesSettings.MoveStepDistance);
				Enemy->TeleportTo(EnemyLocation, Enemy->GetActorRotation());
			}
		}

		RowToMove = (RowToMove + 1) % AllEnemies.Num();
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
