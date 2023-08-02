// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AKCSProjectile.generated.h"

class UProjectileMovementComponent;
class UBoxComponent;

UCLASS()
class KCSTEST_API AAKCSProjectile : public AActor
{
	GENERATED_BODY()

public:
	AAKCSProjectile();

	void Initialize();

protected:

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "KCS|Combat")
	float Damage = 1.0f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> SceneComponent;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> CollisionComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;
};
