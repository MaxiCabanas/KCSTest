// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// KCS Includes
#include "UI/Types/KCSCustomData.h"


// UE Includes
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "KCSGameplayTagMessageSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE(FKCSRequestableData);
DECLARE_MULTICAST_DELEGATE_OneParam(FKCSTagMessage, const FKCSCustomData&);

/**
 * 
 */
UCLASS()
class KCSTEST_API UKCSGameplayTagMessageSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UKCSGameplayTagMessageSubsystem() {};

	static UKCSGameplayTagMessageSubsystem* Get(const UObject* WorldContextObject);

	/** Template method to cast custom data to a given type, checked on editor builds. */
	template<typename T>
	static const T& GetMessageData(const FKCSCustomData& Data)
	{
		check(Data.GetScriptStruct()->IsChildOf(T::StaticStruct()));
		return static_cast<const T&>(Data);
	}

	/** Send an agnostic message. */
	template<typename T>
	static void SendMessage(const UObject* WorldContextObject, const FGameplayTag& Tag, const T& Data);

	/** Sometimes the UI needs to ask for the data, as in racing network condition or initialization orders. */
	static FKCSRequestableData& RegisterRequestableData(const UObject* WorldContextObject, const FGameplayTag& Tag);

	static void RequestData(const UObject* WorldContextObject, const FGameplayTag& Tag);

	FKCSTagMessage& RegisterToTagMessage(const FGameplayTag& Tag);


protected:

	TMap<FGameplayTag, FKCSTagMessage> Listeners;
	TMap<FGameplayTag, FKCSRequestableData> RequestableListeners;
};
