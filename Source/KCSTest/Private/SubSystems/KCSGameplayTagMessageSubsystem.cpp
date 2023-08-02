// Fill out your copyright notice in the Description page of Project Settings.


#include "SubSystems/KCSGameplayTagMessageSubsystem.h"

UKCSGameplayTagMessageSubsystem* UKCSGameplayTagMessageSubsystem::Get(const UObject* WorldContextObject)
{
	if (const UWorld* World = WorldContextObject->GetWorld())
	{
		if (const UGameInstance* GameInstance = World->GetGameInstance())
		{
			return GameInstance->GetSubsystem<UKCSGameplayTagMessageSubsystem>();
		}
	}

	return nullptr;
}

template<typename T>
void UKCSGameplayTagMessageSubsystem::SendMessage(const UObject* WorldContextObject, const FGameplayTag& Tag, const T& Data)
{
	UKCSGameplayTagMessageSubsystem* TagMessageSubsystemInstance = UKCSGameplayTagMessageSubsystem::Get(WorldContextObject);
	
	if (FKCSTagMessage* Listener = TagMessageSubsystemInstance->Listeners.Find(Tag))
	{
		Listener->Broadcast(Data);
	}
}

FKCSRequestableData& UKCSGameplayTagMessageSubsystem::RegisterRequestableData(const UObject* WorldContextObject,
	const FGameplayTag& Tag)
{
	UKCSGameplayTagMessageSubsystem* TagMessageSubsystemInstance = UKCSGameplayTagMessageSubsystem::Get(WorldContextObject);

	return TagMessageSubsystemInstance->RequestableListeners.FindOrAdd(Tag);
}

void UKCSGameplayTagMessageSubsystem::RequestData(const UObject* WorldContextObject, const FGameplayTag& Tag)
{
	UKCSGameplayTagMessageSubsystem* TagMessageSubsystemInstance = UKCSGameplayTagMessageSubsystem::Get(WorldContextObject);

	if (const FKCSRequestableData* MulticastDelegate = TagMessageSubsystemInstance->RequestableListeners.Find(Tag))
	{
		MulticastDelegate->Broadcast();
	}
}

FKCSTagMessage& UKCSGameplayTagMessageSubsystem::RegisterToTagMessage(const FGameplayTag& Tag)
{
	return Listeners.FindOrAdd(Tag);
}

