﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/GameplayTask_KCSPlayerTimeTask.h"

#include "Actors/KCSPlayerState.h"

void UGameplayTask_KCSPlayerTimeTask::Execute(APawn* TaskOwner, const UClass* TaskClass, UGameplayTask_KCSPlayerTimeTask* Default)
{
	UGameplayTask_KCSPlayerTimeTask* TimeTask = NewObject<UGameplayTask_KCSPlayerTimeTask>((UObject*)GetTransientPackage(), TaskClass);
	TimeTask->InitTask(*Cast<IGameplayTaskOwnerInterface>(TaskOwner), TimeTask->GetGameplayTaskDefaultPriority());
	TimeTask->SetUp(Default);
	TimeTask->ReadyForActivation();
}

void UGameplayTask_KCSPlayerTimeTask::SetUp(UGameplayTask_KCSPlayerTimeTask* Default)
{
	CrystalCost = Default->CrystalCost;
	Duration = Default->Duration;
}

bool UGameplayTask_KCSPlayerTimeTask::CanActivate()
{
	const APawn* OwnerPawn = CastChecked<APawn>(GetTaskOwner());
	const AKCSPlayerState* PlayerState = OwnerPawn->GetPlayerState<AKCSPlayerState>();

	return PlayerState->GetCrystalsQuantity() >= CrystalCost;
}

void UGameplayTask_KCSPlayerTimeTask::Activate()
{
	Super::Activate();

	if (CanActivate())
	{
		OnActivate();
		ConsumeCrystals();
	}
	else
	{
		EndTask();
	}
}

void UGameplayTask_KCSPlayerTimeTask::OnActivate()
{
}

void UGameplayTask_KCSPlayerTimeTask::ConsumeCrystals() const
{
	const APawn* OwnerPawn = CastChecked<APawn>(GetTaskOwner());
	AKCSPlayerState* PlayerState = OwnerPawn->GetPlayerState<AKCSPlayerState>();

	PlayerState->RemoveCrystals(CrystalCost);
}
