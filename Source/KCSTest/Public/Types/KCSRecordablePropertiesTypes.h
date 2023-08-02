// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "Curves/CurveVector.h"
#include "Kismet/GameplayStatics.h"
#include "KCSRecordablePropertiesTypes.generated.h"

UENUM(BlueprintType)
enum class EKCSRecordedPropertyType : uint8
{
	NONE = 0,
	Float = 1,
	Vector = 2
};

USTRUCT(BlueprintType)
struct KCSTEST_API FKCSRecordablePropertyParams
{
	GENERATED_BODY()

	/** Reference to the component where to search the property
	 * Set to "RootComponent" to search the property in the RootComponent. Useful for capturing location, rotations, etc. */
	UPROPERTY(EditAnywhere)
	EKCSRecordedPropertyType PropertyType;

	/** The name of the property. */
	UPROPERTY(EditAnywhere)
	FName PropertyName;
};

DECLARE_MULTICAST_DELEGATE_ThreeParams(FKCSRecordablePropertiesEvent, struct FKCSRecordableProperty*, FVector, float);

/** Helper structure to keep record and update a property using a timeline. */
USTRUCT()
struct KCSTEST_API FKCSRecordableProperty
{
	GENERATED_BODY()

public:
	FKCSRecordableProperty() {};
	
	FKCSRecordableProperty(UObject* Outer, UObject* InPropertyOwner, FName InName, EKCSRecordedPropertyType InType)
		: PropertyOwner(InPropertyOwner), Name(InName), Type(InType)
	{
		if (Type == EKCSRecordedPropertyType::Vector)
		{
			VectorCurves = NewObject<UCurveVector>(Outer, UCurveVector::StaticClass(), Name);
			CachedProperty = FindFProperty<FStructProperty>(PropertyOwner->GetClass(), Name);
			return;
		}

		FloatCurve = NewObject<UCurveFloat>(Outer, UCurveFloat::StaticClass(), Name);
		CachedFloatProperty = FindFProperty<FFloatProperty>(PropertyOwner->GetClass(), Name);
	}

	void AddKey()
	{
		const float Time = UGameplayStatics::GetRealTimeSeconds(PropertyOwner);
		
		if (Type == EKCSRecordedPropertyType::Vector)
		{
			const FVector* PropertyValue = CachedProperty->ContainerPtrToValuePtr<FVector>(PropertyOwner);
			VectorCurves->FloatCurves[0].AddKey(Time, PropertyValue->X);
			VectorCurves->FloatCurves[1].AddKey(Time, PropertyValue->Y);
			VectorCurves->FloatCurves[2].AddKey(Time, PropertyValue->Z);
			return;
		}

		const float* PropertyValue = CachedFloatProperty->ContainerPtrToValuePtr<float>(PropertyOwner);
		FloatCurve->FloatCurve.AddKey(Time, *PropertyValue);
	}

	void RegisterToTimeline(FTimeline& Timeline)
	{
		if (Type == EKCSRecordedPropertyType::Vector)
		{
			OnVectorKeyFrameHit = FOnTimelineVectorStatic::CreateRaw(this, &FKCSRecordableProperty::OnVectorPropertyKeyFrameHit);
			Timeline.AddInterpVector(VectorCurves, OnVectorKeyFrameHit);
			Timeline.AddInterpVector(VectorCurves, FOnTimelineVector(), CachedProperty->NamePrivate);
			return;
		}

		OnFloatKeyFrameHit = FOnTimelineFloatStatic::CreateRaw(this, &FKCSRecordableProperty::OnFloatPropertyKeyFrameHit);
		Timeline.AddInterpFloat(FloatCurve, OnFloatKeyFrameHit);
		Timeline.AddInterpFloat(FloatCurve, FOnTimelineFloat(), CachedFloatProperty->NamePrivate);
	}

	FName GetName() const
	{
		return Name;
	}

	void OnVectorPropertyKeyFrameHit(FVector Value)
	{
		OnRecordablePropertyKeyFrameHit.Broadcast(this, Value, 0.0f);
	}

	void OnFloatPropertyKeyFrameHit(float Value)
	{
		OnRecordablePropertyKeyFrameHit.Broadcast(this, FVector::ZeroVector, Value);
	}
	
	FOnTimelineVectorStatic OnVectorKeyFrameHit;

	FOnTimelineFloatStatic OnFloatKeyFrameHit;

	FKCSRecordablePropertiesEvent OnRecordablePropertyKeyFrameHit;

	UPROPERTY(Transient)
	FName Name;

	UPROPERTY(Transient)
	TObjectPtr<UObject> PropertyOwner;
	
	UPROPERTY(Transient)
	EKCSRecordedPropertyType Type = EKCSRecordedPropertyType::NONE;

	UPROPERTY(Transient)
	TObjectPtr<UCurveVector> VectorCurves;

	UPROPERTY(Transient)
	TObjectPtr<UCurveFloat> FloatCurve;

	FStructProperty* CachedProperty = nullptr;
	FFloatProperty* CachedFloatProperty = nullptr;
};

/** A struct that uses a timeline to record properties values of the given object. */
USTRUCT()
struct KCSTEST_API FKCSPropertyRecorderTimeline
{
	GENERATED_BODY()

public:
	FKCSPropertyRecorderTimeline() {};

	FKCSPropertyRecorderTimeline(UObject* InTimelineOwnerObject)
		: TimelineOwnerObject(InTimelineOwnerObject)
	{
		Timeline.SetPropertySetObject(TimelineOwnerObject);
	};

	void Init()
	{
		for (FKCSRecordableProperty& RecordedProperty : RecordedProperties)
		{
			RecordedProperty.RegisterToTimeline(Timeline);
		}
	}

	FKCSRecordableProperty* AddProperty(UObject* Outer, EKCSRecordedPropertyType Type, FName PropertyName)
	{
		return &RecordedProperties.Add_GetRef(FKCSRecordableProperty(Outer, TimelineOwnerObject, PropertyName, Type));
	}

	void TickTimeline(float DeltaTime)
	{
		Timeline.TickTimeline(DeltaTime);
		
		if (!Timeline.IsPlaying())
		{
			for (FKCSRecordableProperty& RecordedProperty : RecordedProperties)
			{
				RecordedProperty.AddKey();
			}
		}
	}

	void DeleteAllFirstKeys()
	{
		for (FKCSRecordableProperty& RecordedProperty : RecordedProperties)
		{
			if (RecordedProperty.Type == EKCSRecordedPropertyType::Vector)
			{
				for (int i = 0; i < 3; i++)
				{
					const FKeyHandle FirstKeyHandle = RecordedProperty.VectorCurves->FloatCurves[i].GetFirstKeyHandle();
					RecordedProperty.VectorCurves->FloatCurves[i].DeleteKey(FirstKeyHandle);
				}			
				continue;
			}
			
			const FKeyHandle FirstKeyHandle = RecordedProperty.FloatCurve->FloatCurve.GetFirstKeyHandle();
			RecordedProperty.FloatCurve->FloatCurve.DeleteKey(FirstKeyHandle);
		}
	}

	void ResetKeys()
	{
		for (FKCSRecordableProperty& RecordedProperty : RecordedProperties)
		{
			if (RecordedProperty.Type == EKCSRecordedPropertyType::Vector)
			{
				RecordedProperty.VectorCurves->ResetCurve();
				continue;
			}

			RecordedProperty.FloatCurve->ResetCurve();
		}
	}

	bool IsOwnedBy(const UObject* Object) const
	{
		return Object == TimelineOwnerObject;
	};

	UObject* GetTimelineOwner() const
	{
		return TimelineOwnerObject;
	}

	const TArray<FKCSRecordableProperty>& GetRecordedProperties() const
	{
		return RecordedProperties;
	}

	UPROPERTY(Transient)
	FTimeline Timeline;

	UPROPERTY(Transient)
	UObject* TimelineOwnerObject = nullptr;
	
	UPROPERTY(Transient)
	TArray<FKCSRecordableProperty> RecordedProperties;

};
