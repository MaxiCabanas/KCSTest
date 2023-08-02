// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/GameplayTask_KCSSlowMoTask.h"

#include "Actors/KCSGameState.h"
#include "Actors/KCSWorldSettings.h"
#include "GameFramework/GameStateBase.h"


void UGameplayTask_KCSSlowMoTask::SetUp(UGameplayTask_KCSPlayerTimeTask* Default)
{
	Super::SetUp(Default);

	const UGameplayTask_KCSSlowMoTask* TaskDefault = CastChecked<UGameplayTask_KCSSlowMoTask>(Default);

	SlowMoDilation = TaskDefault->SlowMoDilation;
}

void UGameplayTask_KCSSlowMoTask::OnActivate()
{
	FTimerHandle TimerHandle;
	const UWorld* World = GetWorld();
	World->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::OnDurationExpired, Duration);
	World->GetGameState<AKCSGameState>()->SetGameStateTimeDilation(SlowMoDilation);
}

void UGameplayTask_KCSSlowMoTask::OnDurationExpired()
{
	const UWorld* World = GetWorld();
	const float GlobalTimeDilation = World->GetWorldSettings()->GetEffectiveTimeDilation();
	World->GetGameState<AKCSGameState>()->SetGameStateTimeDilation(GlobalTimeDilation);
	
	EndTask();
}

