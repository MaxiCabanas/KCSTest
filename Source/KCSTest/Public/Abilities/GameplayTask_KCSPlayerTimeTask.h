// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTask.h"
#include "GameplayTask_KCSPlayerTimeTask.generated.h"

class UInputAction;
/**
 * 
 */
UCLASS(EditInlineNew, DefaultToInstanced)
class KCSTEST_API UGameplayTask_KCSPlayerTimeTask : public UGameplayTask
{
	GENERATED_BODY()

public:

	UFUNCTION(Category = "KCS|GameplayTasks", meta = (DefaultToSelf = "TaskOwner"))
	static void Execute(TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner, const UClass* TaskClass, UGameplayTask_KCSPlayerTimeTask* Default);

	virtual UClass* GetTimeTaskStaticClass() const
	{
		return UGameplayTask_KCSPlayerTimeTask::StaticClass();
	}

protected:

	virtual void SetUp(UGameplayTask_KCSPlayerTimeTask* Default);

	virtual bool CanActivate();

	virtual void Activate() override;

	virtual void OnActivate();

	void ConsumeCrystals() const;
	
	UPROPERTY(EditAnywhere)
	int32 CrystalCost = 0;

	UPROPERTY(EditAnywhere)
	float Duration = 1.0f;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> InputAction
};
