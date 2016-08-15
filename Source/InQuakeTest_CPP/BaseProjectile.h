// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "BaseProjectile.generated.h"

#define COLLISION_PROJECTILE ECC_GameTraceChannel1

UCLASS()
class INQUAKETEST_CPP_API ABaseProjectile : public AActor
{
	GENERATED_BODY()

	/** initial setup */
	virtual void PostInitializeComponents() override;

protected:

	/** movement component */
	UPROPERTY(Replicated, VisibleDefaultsOnly, Category=Projectile)
		UProjectileMovementComponent* MovementComp;

	/** collisions */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
		UCapsuleComponent* CollisionComp;

	/** arrow mesh */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
		UStaticMeshComponent* MeshComp;

public:	
	// Sets default values for this actor's properties
	ABaseProjectile();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

};
