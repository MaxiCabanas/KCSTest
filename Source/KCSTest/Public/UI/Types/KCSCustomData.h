// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KCSCustomData.generated.h"

/**
 * Polymorphic Base class structure that holds custom data to be use as parameters.
 */
USTRUCT(BlueprintType)
struct KCSTEST_API FKCSCustomData
{
	GENERATED_BODY()
	
public:
	FKCSCustomData() {}

	virtual ~FKCSCustomData() {}

	template<typename T>
	const T* GetCustomDataAs() const
	{
		check(GetScriptStruct()->IsChildOf(T::StaticStruct()));
		return static_cast<const T*>(this);
	}

	virtual UScriptStruct* GetScriptStruct() const
	{
		return FKCSCustomData::StaticStruct();
	}
};
