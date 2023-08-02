// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/KCSTimelineComponent.h"

#include "Actors/KCSWorldSettings.h"

static FName ActorLocationPropertyName(TEXT("RelativeLocation"));

UKCSTimelineComponent::UKCSTimelineComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.5f;
}

void UKCSTimelineComponent::BeginPlay()
{
	Super::BeginPlay();

	// Handle specific properties like actor position.
	if (RecordActorLocation)
	{
		USceneComponent* RootComponent = GetOwner()->GetRootComponent();
		FKCSPropertyRecorderTimeline& ActorLocationTimeline = Timelines.Add(RootComponent, FKCSPropertyRecorderTimeline(RootComponent));

		FKCSRecordableProperty* RecordableProperty = ActorLocationTimeline.AddProperty(GetOwner(), EKCSRecordedPropertyType::Vector, ActorLocationPropertyName);
		RecordableProperty->OnRecordablePropertyKeyFrameHit.AddUObject(this, &ThisClass::OnRecordablePropertyKeyFrameHit);
	}

	// Handle generic properties.
	for (const FKCSRecordablePropertyParams& RecordablePropertyParam : RecordablePropertyParams)
	{
		FKCSPropertyRecorderTimeline& PropertyRecorderTimeline = Timelines.FindOrAdd(GetOwner(), FKCSPropertyRecorderTimeline(GetOwner()));
		FKCSRecordableProperty* RecordableProperty = PropertyRecorderTimeline.AddProperty(GetOwner(), RecordablePropertyParam.PropertyType, RecordablePropertyParam.PropertyName);
		RecordableProperty->OnRecordablePropertyKeyFrameHit.AddUObject(this, &ThisClass::OnRecordablePropertyKeyFrameHit);
	}

	OnInternalTimelineFinished.BindUObject(this, &ThisClass::OnTimelinePlayFinished);

	// TODO: Create a struct to use as container so we can make helper functions and avoid this loops everywhere.
	for (TTuple<TObjectPtr<UObject>, FKCSPropertyRecorderTimeline>& TimelinePair : Timelines)
	{
		TimelinePair.Value.Timeline.SetTimelineFinishedFunc(OnInternalTimelineFinished);
		TimelinePair.Value.Init();
	}
}

void UKCSTimelineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (TTuple<TObjectPtr<UObject>, FKCSPropertyRecorderTimeline>& TimelinePair : Timelines)
	{
		TimelinePair.Value.TickTimeline(DeltaTime);
		// TODO: If we need to limit the amount of recorded time here can be a good place.
	}
}

void UKCSTimelineComponent::PlayInReverse()
{
	StartReverseTimestamp = GetWorld()->GetTimeSeconds();

	for (TTuple<TObjectPtr<UObject>, FKCSPropertyRecorderTimeline>& TimelinePair : Timelines)
	{
		TimelinePair.Value.Timeline.ReverseFromEnd();
	}
}

void UKCSTimelineComponent::Stop()
{
	for (TTuple<TObjectPtr<UObject>, FKCSPropertyRecorderTimeline>& TimelinePair : Timelines)
	{
		TimelinePair.Value.Timeline.Stop();
	}
	OnTimelinePlayFinished();
}

void UKCSTimelineComponent::OnRecordablePropertyKeyFrameHit(FKCSRecordableProperty* RecordableProperty, FVector VectorValue, float FloatValue)
{
	if (RecordableProperty->GetName().IsEqual(ActorLocationPropertyName))
	{
		GetOwner()->SetActorLocation(VectorValue);
	}

	// TODO: If we make a container struct, this could be moved inside it.
	if ((GetWorld()->GetTimeSeconds() - StartReverseTimestamp) > ReverseTimeDuration)
	{
		for (TTuple<TObjectPtr<UObject>, FKCSPropertyRecorderTimeline>& TimelinePair : Timelines)
		{
			TimelinePair.Value.Timeline.Stop();
		}
		OnTimelinePlayFinished();
	}
}

void UKCSTimelineComponent::OnTimelinePlayFinished()
{
	int32 FinishedTimelines = 0;
	for (TTuple<TObjectPtr<UObject>, FKCSPropertyRecorderTimeline>& TimelinePair : Timelines)
	{
		if (!TimelinePair.Value.Timeline.IsPlaying())
		{
			FinishedTimelines++;
		}
	}

	if (FinishedTimelines == Timelines.Num())
	{
		LastStopTime = GetWorld()->GetTimeSeconds();
		OnTimelineFinished.Broadcast(this);
	}
}
