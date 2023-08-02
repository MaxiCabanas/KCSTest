// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTask_KCSPlayerTimeTask.h"
#include "Components/KCSTimelineComponent.h"
#include "GameplayTask_KCSReverseTimeTask.generated.h"

class IKCSTimelineOwnerInterface;

/**
 * 
 */
UCLASS(HideCategories = "UGameplayTask_KCSPlayerTimeTask")
class KCSTEST_API UGameplayTask_KCSReverseTimeTask : public UGameplayTask_KCSPlayerTimeTask
{
	GENERATED_BODY()

public:
	
	virtual UClass* GetTimeTaskStaticClass() const override
	{
		return UGameplayTask_KCSReverseTimeTask::StaticClass();
	}
	
protected:

	virtual void SetUp(UGameplayTask_KCSPlayerTimeTask* Default) override;

	UFUNCTION()
	void OnTimelineFinished(UKCSTimelineComponent* TimelineComponent);
	
	virtual void OnActivate() override;

private:

	UPROPERTY()
	TArray<TObjectPtr<AActor>> PendingTimelines;
};
