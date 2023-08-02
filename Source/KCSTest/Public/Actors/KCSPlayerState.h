// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "KCSPlayerState.generated.h"

UCLASS(Config = Game)
class KCSTEST_API AKCSPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	
	virtual void BeginPlay() override;

	void OnLiveLost();
	
	int32 GetCrystalsQuantity() const { return Crystals; }

	UFUNCTION(BlueprintCallable)
	void AddCrystals(int32 CrystalsToAdd);

	void RemoveCrystals(int32 CrystalsToRemove);

	void MatchEnded();

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static void AddScore(UObject* WorldContextObject, int32 ScoreToAdd);

	bool HasAnyLiveLeft() const { return Lives > 0; }

protected:

	void TrySetHighScore(int32 InScore);

	void OnLivesDataRequested();

	void OnHighScoreDataRequested();

	void OnScoreDataRequested();

private:

	UPROPERTY(Config)
	int32 HighScore = 0;

	int32 Crystals = 0;

	int32 Lives = 0;
};
