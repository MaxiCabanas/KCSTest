// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Characters/KCSPawn.h"

#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/FloatingPawnMovement.h"

AKCSPawn::AKCSPawn()
{
 	PrimaryActorTick.bCanEverTick = false;
	SetCanBeDamaged(true);

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	CapsuleComponent->SetShouldUpdatePhysicsVolume(true);
	CapsuleComponent->SetCanEverAffectNavigation(false);
	RootComponent = CapsuleComponent;

	ProjectileSpawnComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ProjectileSpawnComponent"));

	ProjectileSpawnComponent->SetupAttachment(RootComponent);
	ProjectileSpawnComponent->bIsEditorOnly = false;
	ProjectileSpawnComponent->SetHiddenInGame(true);

	FloatingPawnMovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingPawnMovementComponent"));
}

UPawnMovementComponent* AKCSPawn::GetMovementComponent() const
{
	return FloatingPawnMovementComponent;
}

AActor* AKCSPawn::GetProjectileInstigator()
{
	return this;
}

float AKCSPawn::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                           AActor* DamageCauser)
{
	if (!ShouldTakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser))
	{
		return 0.f;
	}

	HitPoints -= DamageAmount;

	// just destroy the pawn for now.
	if (HitPoints <= 0.0f)
	{
		PawnKilled();
		//Destroy();
		SetLifeSpan(0.05f);
	}

	return DamageAmount;
}

void AKCSPawn::PawnKilled()
{
}

void AKCSPawn::Shoot()
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.Owner = this;
	SpawnParameters.Instigator = this;

	const FVector& SpawnLocation = ProjectileSpawnComponent->GetComponentLocation();
	const FRotator& SpawnRotation = ProjectileSpawnComponent->GetComponentRotation();

	if (AActor* ProjectileActor = GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParameters))
	{
		AAKCSProjectile* ProjectileInstance = CastChecked<AAKCSProjectile>(ProjectileActor);
		ProjectileInstance->Initialize();
	}
}
