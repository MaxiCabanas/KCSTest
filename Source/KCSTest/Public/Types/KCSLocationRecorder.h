// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "Curves/CurveVector.h"
#include "KCSLocationRecorder.generated.h"

USTRUCT()
struct FKCSLocationRecorder
{
	GENERATED_BODY()
	
public:

	void Init(UObject* Outer, float InMaxRecordingDuration)
	{
		LocationCurve = NewObject<UCurveVector>(Outer, UCurveVector::StaticClass());

		MaxRecordingDuration = InMaxRecordingDuration;
		
		Timeline.AddInterpVector(LocationCurve, OnLocationKeyFrameHit);
		Timeline.SetTimelineFinishedFunc(OnInternalTimelineStop);
	}

	void TickTimeline(float DeltaTime, const APawn* TargetPawn)
	{
		Timeline.TickTimeline(DeltaTime);

		if (!Timeline.IsPlaying())
		{
			const float TimeSeconds = TargetPawn->GetWorld()->GetTimeSeconds();
		
			const FVector& ActorLocation = TargetPawn->GetActorLocation();
			LocationCurve->FloatCurves[0].AddKey(TimeSeconds, ActorLocation.X);
			LocationCurve->FloatCurves[1].AddKey(TimeSeconds, ActorLocation.Y);
			LocationCurve->FloatCurves[2].AddKey(TimeSeconds, ActorLocation.Z);

			//Handle not saving more keys that needed.
			if ((TimeSeconds - LastStopTime) > 3.0f)
			{
				for (int i = 0; i < 3; i++)
				{
					const FKeyHandle FirstKeyHandle = LocationCurve->FloatCurves[i].GetFirstKeyHandle();
					LocationCurve->FloatCurves[i].DeleteKey(FirstKeyHandle);
				}
			}
		}
	}

	void PlayInReverse(const UObject* ContextWorldObject)
	{
		LastReverseTimestamp = ContextWorldObject->GetWorld()->GetTimeSeconds();
		Timeline.ReverseFromEnd();
	}
	
	void OnLocationKeyframeHit(FVector NewLocation, APawn* TargetPawn)
	{
		TargetPawn->SetActorLocation(NewLocation);

		const float TimeSeconds = TargetPawn->GetWorld()->GetTimeSeconds();
		if ((TimeSeconds - LastReverseTimestamp) > 3.0f)
		{
			Timeline.Stop();
			OnTimelineStopped(TimeSeconds);
		}
	}

	void OnTimelineStopped(float StopTimestamp)
	{
		LocationCurve->FloatCurves[0].Reset();
		LocationCurve->FloatCurves[1].Reset();
		LocationCurve->FloatCurves[2].Reset();

		LastStopTime = StopTimestamp;
	}
	
	FOnTimelineVectorStatic OnLocationKeyFrameHit;
	FOnTimelineEventStatic OnInternalTimelineStop;
	
private:
	
	UPROPERTY(Transient)
	TObjectPtr<UCurveVector> LocationCurve;

	UPROPERTY(Transient)
	FTimeline Timeline;

	float MaxRecordingDuration = 3.0f;

	float LastStopTime = 0.0f;
	float LastReverseTimestamp = 0.0f;
};
