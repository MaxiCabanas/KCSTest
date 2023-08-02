// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "UI/Types/KCSCustomData.h"
#include "KCSIntDisplayWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class KCSTEST_API UKCSIntDisplayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	
protected:
	
	virtual void OnMessageReceived(const FKCSCustomData& CustomData);

	UFUNCTION(BlueprintImplementableEvent, Category = "KCS")
	void BP_OnMessageReceived(int32 Value);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> IntText;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag MessageTagToListen;
};
