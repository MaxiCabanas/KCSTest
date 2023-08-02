// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "Types/KCSRecordablePropertiesTypes.h"

#include "KCSTimelineComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FKCSOnTimelineEvent, UKCSTimelineComponent*, TimelineComponent);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class KCSTEST_API UKCSTimelineComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UKCSTimelineComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool CanPlay() const
	{
		return !Timelines.IsEmpty();
	}

	void SetReverseTimeDuration(float InDuration)
	{
		ReverseTimeDuration = InDuration;
	}

	void PlayInReverse();

	void Stop();

	FOnTimelineEventStatic OnInternalTimelineFinished;

	UPROPERTY(BlueprintAssignable)
	FKCSOnTimelineEvent OnTimelineFinished;

protected:
	
	void OnRecordablePropertyKeyFrameHit(FKCSRecordableProperty* RecordableProperty, FVector VectorValue, float FloatValue);

	void OnTimelinePlayFinished();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "KCS|RecordableProperties")
	bool RecordActorLocation = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "KCS|RecordableProperties")
	TArray<FKCSRecordablePropertyParams> RecordablePropertyParams;
	
private:

	// TODO: This could be replaced by a container structure with helper functions.
	UPROPERTY(Transient)
	TMap<TObjectPtr<UObject>, FKCSPropertyRecorderTimeline> Timelines;

	UPROPERTY(Transient)
	float LastStopTime = 0.0f;

	UPROPERTY(Transient)
	float StartReverseTimestamp = 0.0f;

	UPROPERTY(Transient)
	float ReverseTimeDuration = 3.0f;

};
