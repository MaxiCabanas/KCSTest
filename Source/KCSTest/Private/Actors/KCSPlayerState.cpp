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
#define NOTIFY_HIGHSCORE_TO_UI() UKCSGameplayTagMessageSubsystem::SendMessage<FKCSIntData>(GetWorld(), TAG_UI_Message_PlayerHighScoreUpdated, FKCSIntData(HighScore));

void AKCSPlayerState::BeginPlay()
{
	Super::BeginPlay();

	const AKCSWorldSettings* WorldSettings = CastChecked<AKCSWorldSettings>(GetWorld()->GetWorldSettings());
	Lives = WorldSettings->GetInitialPlayerLives();

	LoadConfig();

	FKCSRequestableData& LiveRequestableData = UKCSGameplayTagMessageSubsystem::RegisterRequestableData(this, TAG_UI_Message_PlayerLivesUpdated);
	LiveRequestableData.AddUObject(this, &ThisClass::OnLivesDataRequested);

	FKCSRequestableData& HighScoreRequestableData = UKCSGameplayTagMessageSubsystem::RegisterRequestableData(this, TAG_UI_Message_PlayerHighScoreUpdated);
	HighScoreRequestableData.AddUObject(this, &ThisClass::OnHighScoreDataRequested);

	FKCSRequestableData& ScoreRequestableData = UKCSGameplayTagMessageSubsystem::RegisterRequestableData(this, TAG_UI_Message_PlayerScoreUpdated);
	ScoreRequestableData.AddUObject(this, &ThisClass::OnHighScoreDataRequested);
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

void AKCSPlayerState::MatchEnded()
{
	const int32 CurrentScore = FMath::RoundHalfToEven(GetScore());
	HighScore = FMath::Max(HighScore, CurrentScore);
	SaveConfig();
}

void AKCSPlayerState::AddScore(UObject* WorldContextObject, int32 ScoreToAdd)
{
	const UWorld* World = WorldContextObject->GetWorld();
	const APlayerController* PlayerController = World->GetFirstPlayerController();

	AKCSPlayerState* PlayerState = PlayerController->GetPlayerState<AKCSPlayerState>();
	
	const float NewScore = PlayerState->GetScore() + ScoreToAdd;
	PlayerState->SetScore(NewScore);
	PlayerState->TrySetHighScore(NewScore);
	
	NOTIFY_SCORE_TO_UI()
}

void AKCSPlayerState::TrySetHighScore(int32 InScore)
{
	if (InScore > HighScore)
	{
		HighScore = InScore;
		NOTIFY_HIGHSCORE_TO_UI();
	}
}

void AKCSPlayerState::OnLivesDataRequested()
{
	NOTIFY_LIVES_TO_UI();
}

void AKCSPlayerState::OnHighScoreDataRequested()
{
	NOTIFY_HIGHSCORE_TO_UI();
}

void AKCSPlayerState::OnScoreDataRequested()
{
	UKCSGameplayTagMessageSubsystem::SendMessage<FKCSIntData>(GetWorld(), TAG_UI_Message_PlayerScoreUpdated, FKCSIntData(HighScore));
}

