// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/KCSPlayerController.h"

// KCS Includes
#include "Abilities/GameplayTask_KCSPlayerTimeTask.h"

// UE Includes
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameplayTasksComponent.h"
#include "Actors/Characters/KCSPlayerPawn.h"

AKCSPlayerController::AKCSPlayerController()
{
	GameplayTasksComponent = CreateDefaultSubobject<UGameplayTasksComponent>(TEXT("GameplayTasksComponent"));
}

UGameplayTasksComponent* AKCSPlayerController::GetGameplayTasksComponent(const UGameplayTask& Task) const
{
	return GameplayTasksComponent;
}

void AKCSPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	UEnhancedInputLocalPlayerSubsystem* EISubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	EISubsystem->ClearAllMappings();
	EISubsystem->AddMappingContext(GameplayMappingContext, 0);

	EnableInput(this);
}

void AKCSPlayerController::PawnPendingDestroy(APawn* inPawn)
{
	Super::PawnPendingDestroy(inPawn);

	DisableInput(this);
	OnPawnDestroyed.Broadcast();
}

void AKCSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this, &ThisClass::TryToShoot);
	
	EnhancedInputComponent->BindAction(MoveLeftAction, ETriggerEvent::Triggered, this, &ThisClass::ProcessMoveInput, &LastMoveLeftValue);
	EnhancedInputComponent->BindAction(MoveRightAction, ETriggerEvent::Triggered, this, &ThisClass::ProcessMoveInput, &LastMoveRightValue);

	// Called with input value 0;
	EnhancedInputComponent->BindAction(MoveLeftAction, ETriggerEvent::Completed, this, &ThisClass::ProcessMoveInput, &LastMoveLeftValue);
	EnhancedInputComponent->BindAction(MoveRightAction, ETriggerEvent::Completed, this, &ThisClass::ProcessMoveInput, &LastMoveRightValue);

	for (FKCSInputGameplayTask& Task : InputTasks)
	{
		if (Task.InputAction && Task.Task)
		{
			EnhancedInputComponent->BindAction(Task.InputAction, ETriggerEvent::Triggered, this, &ThisClass::ActivateGameplayTask, Task.Task);
		}
	}
}

void AKCSPlayerController::ActivateGameplayTask(TObjectPtr<UGameplayTask_KCSPlayerTimeTask> TaskDefault)
{
	if (bIsThereAnyTaskInExecution)
	{
		return;
	}
	
	UGameplayTask_KCSPlayerTimeTask::Execute(this, TaskDefault->GetTimeTaskStaticClass(), TaskDefault);
}

void AKCSPlayerController::ProcessMoveInput(const FInputActionValue& InputActionValue, float* DirectionLastValue)
{
	*DirectionLastValue = InputActionValue.GetMagnitude();
	
	const FVector MovementDirection(LastMoveLeftValue + LastMoveRightValue, 0.0f, 0.0f);

	GetPawn()->AddMovementInput(MovementDirection, 1.0, false);
}

void AKCSPlayerController::TryToShoot()
{
	const UWorld* World = GetWorld();
	
	const float GameTimeSinceCreation = World->GetRealTimeSeconds();
	
	if (GameTimeSinceCreation >= (LastShotTimestamp + ShotCooldown))
	{
		AKCSPlayerPawn* PlayerPawn = CastChecked<AKCSPlayerPawn>(GetPawn());
		PlayerPawn->Shoot();
		
		LastShotTimestamp = World->GetRealTimeSeconds();
	}
}
