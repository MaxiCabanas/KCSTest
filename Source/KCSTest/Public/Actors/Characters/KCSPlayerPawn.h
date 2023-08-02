// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// KCS includes
#include "KCSPawn.h"

// UE Includes
#include "CoreMinimal.h"
#include "GameplayTaskOwnerInterface.h"
#include "InputActionValue.h"
#include "KCSPlayerPawn.generated.h"

class UCameraComponent;
class UGameplayTasksComponent;
class UGameplayTask_KCSPlayerTimeTask;
class UInputAction;
class UInputMappingContext;

/** Wrapper struct that pairs a InputAction to a GameplayTask. */
USTRUCT(BlueprintType)
struct FKCSInputGameplayTask
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> InputAction;

	UPROPERTY(Instanced, EditDefaultsOnly, meta = (ShowOnlyInnerProperties))
	TObjectPtr<UGameplayTask_KCSPlayerTimeTask> Task;
};

UCLASS()
class KCSTEST_API AKCSPlayerPawn : public AKCSPawn, public IGameplayTaskOwnerInterface
{
	GENERATED_BODY()

public:
	AKCSPlayerPawn();
	
	virtual void PawnClientRestart() override;

	// ~Begin IGameplayTaskOwnerInterface
	virtual UGameplayTasksComponent* GetGameplayTasksComponent(const UGameplayTask& Task) const override;
	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override
	{
		bIsThereAnyTaskInExecution = true;
	}
	virtual void OnGameplayTaskDeactivated(UGameplayTask& Task) override
	{
		bIsThereAnyTaskInExecution = false;
	}
	// ~End GameplayTaskOwnerInterface
	
protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	//void ExecuteAbility(UKCSAbility* Ability);
	void ActivateGameplayTask(TObjectPtr<UGameplayTask_KCSPlayerTimeTask>  TaskDefault);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "KCS|Input")
	TObjectPtr<UInputAction> MoveLeftAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "KCS|Input")
	TObjectPtr<UInputAction> MoveRightAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "KCS|Input")
	TObjectPtr<UInputAction> ShootAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "KCS|Input")
	TObjectPtr<UInputMappingContext> GameplayMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "KCS|Input")
	TArray<FKCSInputGameplayTask> InputTasks;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "KCS|Input")
	float ShotCooldown = 0.5f;
	
private:

	void TryToShoot();

	void ProcessMoveInput(const FInputActionValue& InputActionValue, float* DirectionLastValue);

	UPROPERTY(Transient)
	TArray<TObjectPtr<UGameplayTask_KCSPlayerTimeTask>> PlayerTimeTasks;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UGameplayTasksComponent> GameplayTasksComponent;

	float LastShotTimestamp = 0.0f;

	// Handle opposite directions from inputs
	float LastMoveLeftValue = 0.0f;
	float LastMoveRightValue = 0.0f;

	bool bIsThereAnyTaskInExecution = false;
};
