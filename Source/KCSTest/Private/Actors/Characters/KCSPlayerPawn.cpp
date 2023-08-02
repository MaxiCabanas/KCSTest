// Fill out your copyright notice in the Description page of Project Settings.

// KCS includes
#include "Actors/Characters/KCSPlayerPawn.h"
#include "Abilities/GameplayTask_KCSPlayerTimeTask.h"

// UE Includes
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameplayTasksComponent.h"

AKCSPlayerPawn::AKCSPlayerPawn()
{
	ShotDirection = FVector(0,1,0);
	GameplayTasksComponent = CreateDefaultSubobject<UGameplayTasksComponent>(TEXT("GameplayTasksComponent"));
}

void AKCSPlayerPawn::PawnClientRestart()
{
	Super::PawnClientRestart();

	const APlayerController* PC = CastChecked<APlayerController>(GetController());
	UEnhancedInputLocalPlayerSubsystem* EISubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());

	EISubsystem->ClearAllMappings();
	EISubsystem->AddMappingContext(GameplayMappingContext, 0);
}

UGameplayTasksComponent* AKCSPlayerPawn::GetGameplayTasksComponent(const UGameplayTask& Task) const
{
	return GameplayTasksComponent;
}

void AKCSPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

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

void AKCSPlayerPawn::ActivateGameplayTask(TObjectPtr<UGameplayTask_KCSPlayerTimeTask> TaskDefault)
{
	if (bIsThereAnyTaskInExecution)
	{
		return;
	}
	
	UGameplayTask_KCSPlayerTimeTask::Execute(this, TaskDefault->GetTimeTaskStaticClass(), TaskDefault);
}

void AKCSPlayerPawn::TryToShoot()
{
	const UWorld* World = GetWorld();
	
	const float GameTimeSinceCreation = World->GetRealTimeSeconds();
	
	if (GameTimeSinceCreation >= (LastShotTimestamp + ShotCooldown))
	{
 		Shoot();
		LastShotTimestamp = World->GetRealTimeSeconds();
	}
}

void AKCSPlayerPawn::ProcessMoveInput(const FInputActionValue& InputActionValue, float* DirectionLastValue)
{
	*DirectionLastValue = InputActionValue.GetMagnitude();
	
	const FVector MovementDirection(LastMoveLeftValue + LastMoveRightValue, 0.0f, 0.0f);
	AddMovementInput(MovementDirection, 1.0, false);
}
