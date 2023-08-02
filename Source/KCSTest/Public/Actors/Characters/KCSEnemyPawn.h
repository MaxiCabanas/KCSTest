// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KCSPawn.h"
#include "Interfaces/KCSTimelineOwnerInterface.h"
#include "KCSEnemyPawn.generated.h"

class UKCSTimelineComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FKCSEnemyPawnEvent, AKCSEnemyPawn*, EnemyPawn);

UCLASS()
class KCSTEST_API AKCSEnemyPawn : public AKCSPawn, public IKCSTimelineOwnerInterface
{
	GENERATED_BODY()

public:
	AKCSEnemyPawn();

	// ~Begin IKCSTimelineOwnerInterface
	virtual UKCSTimelineComponent* GetTimelineComponent() override { return TimelineComponent; }
	// ~End IKCSTimelineOwnerInterface
	
	void SetIndex(FIntVector2 InIndex);

	void GetIndex(FIntVector2& OutIndex) const;

	FKCSEnemyPawnEvent OnEnemyKilled;
	
protected:

	virtual void PawnKilled() override;

	void TryDropCrystal() const;

	UFUNCTION(BlueprintCallable)
	FVector GetEnemyIndex() const { return FVector(Index.X, Index.Y, 0.0f); }

	/** The chance for this enemy to drop a Crystal when dies. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "KCS|Enemy", meta = (UIMin = 0, ClampMin = 0, UIMax = 100.0f, ClampMax = 100.0f, Units = Percent))
	float CrystalDropChance = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "KCS|Enemy")
	TSubclassOf<AActor> CrystalClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UKCSTimelineComponent* TimelineComponent;

private:

	UPROPERTY()
	FIntVector2 Index;
};
