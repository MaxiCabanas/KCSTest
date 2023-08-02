// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Characters/KCSEnemyPawn.h"

#include "Actors/KCSWorldSettings.h"


AKCSEnemyPawn::AKCSEnemyPawn()
{
	ShotDirection = FVector(0,-1,0);

	TimelineComponent = CreateDefaultSubobject<UKCSTimelineComponent>(TEXT("TimelineComponent"));
}

void AKCSEnemyPawn::SetIndex(FIntVector2 InIndex)
{
	Index = InIndex;
}

void AKCSEnemyPawn::GetIndex(FIntVector2& OutIndex) const
{
	OutIndex.X = Index.X;
	OutIndex.Y = Index.Y;
}

void AKCSEnemyPawn::PawnKilled()
{
	Super::PawnKilled();

	TimelineComponent->Stop();
	OnEnemyKilled.Broadcast(this);
	TryDropCrystal();
}

void AKCSEnemyPawn::TryDropCrystal() const
{
	const float RandNum = CrystalDropChance > 0.0f ? FMath::RandRange(0.0f, 100.0f) : -1.0f;
	if (RandNum <= CrystalDropChance)
	{
		GetWorld()->SpawnActor<AActor>(CrystalClass, GetActorLocation(), FRotator(0.0f, -90.0f, 0.0f), FActorSpawnParameters());
	}
}
