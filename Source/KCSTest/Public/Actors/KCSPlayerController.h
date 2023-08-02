// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTaskOwnerInterface.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "KCSPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FKCSPlayerConstrollerEvent);

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
class KCSTEST_API AKCSPlayerController : public APlayerController, public IGameplayTaskOwnerInterface
{
	GENERATED_BODY()

public:
	AKCSPlayerController();
	
	// ~Begin IGameplayTaskOwnerInterface
	virtual UGameplayTasksComponent* GetGameplayTasksComponent(const UGameplayTask& Task) const override;

	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override { bIsThereAnyTaskInExecution = true;	}
	
	virtual void OnGameplayTaskDeactivated(UGameplayTask& Task) override { bIsThereAnyTaskInExecution = false; }

	virtual AActor* GetGameplayTaskAvatar(const UGameplayTask* Task) const override { return GetPawn(); }
	// ~End GameplayTaskOwnerInterface

	FKCSPlayerConstrollerEvent OnPawnDestroyed;
	
protected:

	virtual void OnPossess(APawn* InPawn) override;
	
	virtual void SetupInputComponent() override;

	void ActivateGameplayTask(TObjectPtr<UGameplayTask_KCSPlayerTimeTask>  TaskDefault);

	void ProcessMoveInput(const FInputActionValue& InputActionValue, float* DirectionLastValue);

	void TryToShoot();

	virtual void PawnPendingDestroy(APawn* inPawn) override;
	
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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UGameplayTasksComponent> GameplayTasksComponent;

	float LastShotTimestamp = 0.0f;

	// Handle opposite directions from inputs
	float LastMoveLeftValue = 0.0f;
	float LastMoveRightValue = 0.0f;
	
	bool bIsThereAnyTaskInExecution = false;
};
