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

	TSet<TScriptInterface<IKCSTimelineOwnerInterface>> AllTimelineOwners;
	GameState->GetAllTimelineOwners(AllTimelineOwners);

	for (const TScriptInterface<IKCSTimelineOwnerInterface>& TimelineOwner : AllTimelineOwners)
	{
		UKCSTimelineComponent* TimelineComponent = TimelineOwner->GetTimelineComponent();

		if (!TimelineComponent->CanPlay())
		{
			continue;
		}

		TimelineComponent->OnTimelineFinished.AddUniqueDynamic(this, &ThisClass::OnTimelineFinished);
		TimelineComponent->SetReverseTimeDuration(Duration);
		TimelineComponent->PlayInReverse();
		PendingTimelines.Add(TimelineComponent);
	}
}

void UGameplayTask_KCSReverseTimeTask::OnTimelineFinished(UKCSTimelineComponent* TimelineComponent)
{
	PendingTimelines.Remove(TimelineComponent);

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
