// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KCSCustomData.h"
#include "KCSIntData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct KCSTEST_API FKCSIntData : public FKCSCustomData
{
	GENERATED_BODY()
	
public:
	FKCSIntData() {}

	FKCSIntData(int32 InValue) : Value(InValue) {}
	
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FKCSIntData::StaticStruct();
	}

	UPROPERTY(BlueprintReadWrite)
	int32 Value = 0;
};
