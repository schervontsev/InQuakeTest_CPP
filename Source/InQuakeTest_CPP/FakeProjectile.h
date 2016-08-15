// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BaseProjectile.h"
#include "FakeProjectile.generated.h"

/**
 * 
 */
UCLASS()
class INQUAKETEST_CPP_API AFakeProjectile : public ABaseProjectile
{
	GENERATED_BODY()
	
public:
	AFakeProjectile();

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;
	
	// overlap
	virtual void NotifyActorBeginOverlap(AActor * OtherActor) override;
};
