// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/KCSTimelineComponent.h"

#include "Actors/KCSGameState.h"
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
	
	if (RecordActorLocation)
	{
		USceneComponent* RootComponent = GetOwner()->GetRootComponent();
		FKCSPropertyRecorderTimeline& ActorLocationTimeline = Timelines.Add(RootComponent, FKCSPropertyRecorderTimeline(RootComponent));

		FKCSRecordableProperty* RecordableProperty = ActorLocationTimeline.AddProperty(GetOwner(), EKCSRecordedPropertyType::Vector, ActorLocationPropertyName);
		RecordableProperty->OnRecordablePropertyKeyFrameHit.AddUObject(this, &ThisClass::OnRecordablePropertyKeyFrameHit);
	}
	
	for (const FKCSRecordablePropertyParams& RecordablePropertyParam : RecordablePropertyParams)
	{
		FKCSPropertyRecorderTimeline& PropertyRecorderTimeline = Timelines.FindOrAdd(GetOwner(), FKCSPropertyRecorderTimeline(GetOwner()));
		FKCSRecordableProperty* RecordableProperty = PropertyRecorderTimeline.AddProperty(GetOwner(), RecordablePropertyParam.PropertyType, RecordablePropertyParam.PropertyName);
		RecordableProperty->OnRecordablePropertyKeyFrameHit.AddUObject(this, &ThisClass::OnRecordablePropertyKeyFrameHit);
	}

	OnInternalTimelineFinished.BindUObject(this, &ThisClass::OnTimelinePlayFinished);
	
	for (TTuple<TObjectPtr<UObject>, FKCSPropertyRecorderTimeline>& TimelinePair : Timelines)
	{
		TimelinePair.Value.Timeline.SetTimelineFinishedFunc(OnInternalTimelineFinished);
		TimelinePair.Value.Init();
	}

	const AKCSWorldSettings* WorldSettings = CastChecked<AKCSWorldSettings>(GetWorld()->GetWorldSettings());
	TimeReverseAmount = WorldSettings->GetTimeReverseAmount();
}

void UKCSTimelineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (TTuple<TObjectPtr<UObject>, FKCSPropertyRecorderTimeline>& TimelinePair : Timelines)
	{
		TimelinePair.Value.TickTimeline(DeltaTime);
	
		if (!TimelinePair.Value.Timeline.IsPlaying())
		{
			const float TimeSeconds = GetWorld()->GetTimeSeconds();
			
			//Handle not saving more keys that needed.
			// if ((TimeSeconds - LastStopTime) > TimeReverseAmount)
			// {
			// 	TimelinePair.Value.DeleteAllFirstKeys();
			// }
		}
	}
}

bool UKCSTimelineComponent::CanPlay() const
{
	return !Timelines.IsEmpty();
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
}

void UKCSTimelineComponent::OnRecordablePropertyKeyFrameHit(FKCSRecordableProperty* RecordableProperty, FVector VectorValue, float FloatValue)
{
	if (RecordableProperty->GetName().IsEqual(ActorLocationPropertyName))
	{
		GetOwner()->SetActorLocation(VectorValue);
	}

	if ((GetWorld()->GetTimeSeconds() - StartReverseTimestamp) > TimeReverseAmount)
	{
		for (TTuple<TObjectPtr<UObject>, FKCSPropertyRecorderTimeline>& TimelinePair : Timelines)
		{
			TimelinePair.Value.Timeline.Stop();
		}
		OnTimelinePlayFinished();
	}
}

void UKCSTimelineComponent::OnVectorKeyFrameHit(FVector KeyFrameLocation)
{

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
