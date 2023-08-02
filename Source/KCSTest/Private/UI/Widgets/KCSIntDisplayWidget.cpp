// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/KCSIntDisplayWidget.h"

#include "SubSystems/KCSGameplayTagMessageSubsystem.h"
#include "UI/Types/KCSIntData.h"

void UKCSIntDisplayWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	UKCSGameplayTagMessageSubsystem* TagMessageSubsystem = UKCSGameplayTagMessageSubsystem::Get(this);
	TagMessageSubsystem->RegisterToTagMessage(MessageTagToListen).AddUObject(this, &ThisClass::OnMessageReceived);
	TagMessageSubsystem->RequestData(this, MessageTagToListen);
}

void UKCSIntDisplayWidget::OnMessageReceived(const FKCSCustomData& CustomData)
{
	const FKCSIntData& IntData = *CustomData.GetCustomDataAs<FKCSIntData>();
	BP_OnMessageReceived(IntData.Value);
}
