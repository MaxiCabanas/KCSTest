// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTask_KCSPlayerTimeTask.h"
#include "GameplayTask_KCSSlowMoTask.generated.h"

/**
 * 
 */
UCLASS()
class KCSTEST_API UGameplayTask_KCSSlowMoTask : public UGameplayTask_KCSPlayerTimeTask
{
	GENERATED_BODY()

public:
	
	virtual UClass* GetTimeTaskStaticClass() const override
	{
		return UGameplayTask_KCSSlowMoTask::StaticClass();
	}
	
protected:

	virtual void SetUp(UGameplayTask_KCSPlayerTimeTask* Default) override;
	
	virtual void OnActivate() override;

	void OnDurationExpired();

	UPROPERTY(EditAnywhere)
	float SlowMoDilation = 0.1f;
};
