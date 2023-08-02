// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/KCSPlayerState.h"

// KCS Includes
#include "Actors/KCSWorldSettings.h"
#include "SubSystems/KCSGameplayTagMessageSubsystem.h"
#include "UI/Types/KCSIntData.h"
#include "UI/Types/KCSUITags.h"

#define NOTIFY_LIVES_TO_UI() UKCSGameplayTagMessageSubsystem::SendMessage<FKCSIntData>(GetWorld(), TAG_UI_Message_PlayerLivesUpdated, FKCSIntData(Lives));
#define NOTIFY_CRYSTALS_TO_UI() UKCSGameplayTagMessageSubsystem::SendMessage<FKCSIntData>(GetWorld(), TAG_UI_Message_PlayerCrystalsUpdated, FKCSIntData(Crystals));
#define NOTIFY_SCORE_TO_UI() UKCSGameplayTagMessageSubsystem::SendMessage<FKCSIntData>(WorldContextObject, TAG_UI_Message_PlayerScoreUpdated, FKCSIntData(NewScore));

void AKCSPlayerState::BeginPlay()
{
	Super::BeginPlay();

	const AKCSWorldSettings* WorldSettings = CastChecked<AKCSWorldSettings>(GetWorld()->GetWorldSettings());
	Lives = WorldSettings->GetInitialPlayerLives();

	FKCSRequestableData& RequestableData = UKCSGameplayTagMessageSubsystem::RegisterRequestableData(this, TAG_UI_Message_PlayerLivesUpdated);
	RequestableData.AddUObject(this, &ThisClass::OnLivesDataRequested);
}

void AKCSPlayerState::OnLiveLost()
{
	Lives--;
	NOTIFY_LIVES_TO_UI()
}

void AKCSPlayerState::AddCrystals(int32 CrystalsToAdd)
{
	ensure(CrystalsToAdd > 0);
	Crystals += CrystalsToAdd;
	NOTIFY_CRYSTALS_TO_UI()
}

void AKCSPlayerState::RemoveCrystals(int32 CrystalsToRemove)
{
	Crystals -= FMath::Max(CrystalsToRemove, 0);
	NOTIFY_CRYSTALS_TO_UI()
}

void AKCSPlayerState::AddScore(UObject* WorldContextObject, int32 ScoreToAdd)
{
	const UWorld* World = WorldContextObject->GetWorld();
	const APlayerController* PlayerController = World->GetFirstPlayerController();

	APlayerState* PlayerState = PlayerController->GetPlayerState<APlayerState>();
	
	const float NewScore = PlayerState->GetScore() + ScoreToAdd;
	PlayerState->SetScore(NewScore);
	NOTIFY_SCORE_TO_UI()
}

void AKCSPlayerState::OnLivesDataRequested()
{
	NOTIFY_LIVES_TO_UI();
}

