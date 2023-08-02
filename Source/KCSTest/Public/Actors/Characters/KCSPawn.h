// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// KCS Includes
#include "Actors/AKCSProjectile.h"

// UE Includes
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "KCSPawn.generated.h"

class UArrowComponent;
class UCapsuleComponent;
class UFloatingPawnMovement;

UCLASS()
class KCSTEST_API AKCSPawn : public APawn
{
	GENERATED_BODY()

public:
	AKCSPawn();

	virtual UPawnMovementComponent* GetMovementComponent() const override;
	
	void Shoot();
	
protected:

	virtual AActor* GetProjectileInstigator();

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void PawnKilled();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "KCS|Combat")
	float HitPoints = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "KCS|Combat")
	TSubclassOf<AAKCSProjectile> ProjectileClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UArrowComponent> ProjectileSpawnComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCapsuleComponent> CapsuleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UFloatingPawnMovement> FloatingPawnMovementComponent;

	FVector ShotDirection = FVector::Zero();
};
