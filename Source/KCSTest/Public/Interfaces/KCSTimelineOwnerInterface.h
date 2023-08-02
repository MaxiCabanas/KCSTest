// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/KCSTimelineComponent.h"
#include "UObject/Interface.h"
#include "KCSTimelineOwnerInterface.generated.h"

UINTERFACE()
class UKCSTimelineOwnerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class KCSTEST_API IKCSTimelineOwnerInterface
{
	GENERATED_BODY()

public:
	virtual UKCSTimelineComponent* GetTimelineComponent() = 0;
};
