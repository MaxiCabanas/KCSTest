// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/KCSPlayerState.h"

// KCS Includes
#include "Actors/KCSWorldSettings.h"
#include "SubSystems/KCSGameplayTagMessageSubsystem.h"
#include "UI/Types/KCSIntData.h"
#include "UI/Types/KCSUITags.h"

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
	ScoreRequestableData.AddUObject(this, &ThisClass::OnScoreDataRequested);
}

void AKCSPlayerState::OnLiveLost()
{
	Lives--;
	UKCSGameplayTagMessageSubsystem::SendMessage<FKCSIntData>(GetWorld(), TAG_UI_Message_PlayerLivesUpdated, FKCSIntData(Lives));
}

void AKCSPlayerState::AddCrystals(int32 CrystalsToAdd)
{
	ensure(CrystalsToAdd > 0);
	Crystals += CrystalsToAdd;
	UKCSGameplayTagMessageSubsystem::SendMessage<FKCSIntData>(GetWorld(), TAG_UI_Message_PlayerCrystalsUpdated, FKCSIntData(Crystals));
}

void AKCSPlayerState::RemoveCrystals(int32 CrystalsToRemove)
{
	Crystals -= FMath::Max(CrystalsToRemove, 0);
	UKCSGameplayTagMessageSubsystem::SendMessage<FKCSIntData>(GetWorld(), TAG_UI_Message_PlayerCrystalsUpdated, FKCSIntData(Crystals));
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

	const int32 ScoreAsInt = FMath::RoundHalfToEven(NewScore);
	UKCSGameplayTagMessageSubsystem::SendMessage<FKCSIntData>(WorldContextObject, TAG_UI_Message_PlayerScoreUpdated, FKCSIntData(ScoreAsInt));
}

void AKCSPlayerState::TrySetHighScore(int32 InScore)
{
	if (InScore > HighScore)
	{
		HighScore = InScore;
		UKCSGameplayTagMessageSubsystem::SendMessage<FKCSIntData>(GetWorld(), TAG_UI_Message_PlayerHighScoreUpdated, FKCSIntData(HighScore));
	}
}

void AKCSPlayerState::OnLivesDataRequested()
{
	UKCSGameplayTagMessageSubsystem::SendMessage<FKCSIntData>(GetWorld(), TAG_UI_Message_PlayerLivesUpdated, FKCSIntData(Lives));
}

void AKCSPlayerState::OnHighScoreDataRequested()
{
	UKCSGameplayTagMessageSubsystem::SendMessage<FKCSIntData>(GetWorld(), TAG_UI_Message_PlayerHighScoreUpdated, FKCSIntData(HighScore));
}

void AKCSPlayerState::OnScoreDataRequested()
{
	const int32 ScoreAsInt = FMath::RoundHalfToEven(GetScore());
	UKCSGameplayTagMessageSubsystem::SendMessage<FKCSIntData>(GetWorld(), TAG_UI_Message_PlayerScoreUpdated, FKCSIntData(ScoreAsInt));
}

