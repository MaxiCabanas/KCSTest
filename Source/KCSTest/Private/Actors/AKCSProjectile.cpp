// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/AKCSProjectile.h"

#include "Components/BoxComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/ProjectileMovementComponent.h"


AAKCSProjectile::AAKCSProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;
	
	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetupAttachment(RootComponent);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
}

void AAKCSProjectile::Initialize()
{
	CollisionComponent->IgnoreActorWhenMoving(GetOwner(), true);
}

void AAKCSProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	AController* EventInstigator = GetInstigator()->GetController();
	
	OtherActor->TakeDamage(Damage, FDamageEvent(), EventInstigator, GetOwner());

	if (APlayerController* PlayerController = Cast<APlayerController>(EventInstigator))
	{
		PlayerController->InstigatedAnyDamage(Damage, nullptr, OtherActor, GetOwner());
	}

	// We destroy the projectile for now.
	Destroy();
}
