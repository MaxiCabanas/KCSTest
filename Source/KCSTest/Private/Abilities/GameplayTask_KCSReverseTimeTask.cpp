// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/GameplayTask_KCSReverseTimeTask.h"

#include "Actors/KCSGameState.h"
#include "Interfaces/KCSTimelineOwnerInterface.h"

void UGameplayTask_KCSReverseTimeTask::SetUp(UGameplayTask_KCSPlayerTimeTask* Default)
{
	Super::SetUp(Default);
}

void UGameplayTask_KCSReverseTimeTask::OnActivate()
{
	Super::OnActivate();

	AKCSGameState* GameState = GetWorld()->GetGameState<AKCSGameState>();
	GameState->SetGameStateTickEnabled(false);

	TSet<TObjectPtr<AActor>> AllEnemies;
	GameState->GetAllEnemiesActors(AllEnemies);

	for (const TObjectPtr<AActor> Enemy : AllEnemies)
	{
		IKCSTimelineOwnerInterface* TimelineOwner = CastChecked<IKCSTimelineOwnerInterface>(Enemy); 
		UKCSTimelineComponent* TimelineComponent = TimelineOwner->GetTimelineComponent();

		if (!TimelineComponent->CanPlay())
		{
			continue;
		}

		TimelineComponent->OnTimelineFinished.AddUniqueDynamic(this, &ThisClass::OnTimelineFinished);
		TimelineComponent->PlayInReverse();
		PendingTimelines.Add(Enemy);
	}
}

void UGameplayTask_KCSReverseTimeTask::OnTimelineFinished(UKCSTimelineComponent* TimelineComponent)
{
	PendingTimelines.Remove(TimelineComponent->GetOwner());
	if (PendingTimelines.IsEmpty())
	{
		if (const UWorld* World = GetWorld())
		{
			AKCSGameState* GameState = World->GetGameState<AKCSGameState>();
			GameState->SetGameStateTickEnabled(true);
			EndTask();
		}
	}
}
