// Fill out your copyright notice in the Description page of Project Settings.

#include "InQuakeTest_CPP.h"
#include "FakeProjectile.h"

AFakeProjectile::AFakeProjectile(): ABaseProjectile()
{
	MovementComp->InitialSpeed = MovementComp->InitialSpeed * 0.5f;
	MovementComp->MaxSpeed = MovementComp->MaxSpeed * 0.5f;
}

void AFakeProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
}

void AFakeProjectile::NotifyActorBeginOverlap(AActor * OtherActor)
{
	if (OtherActor != GetInstigator()) {
		MovementComp->StopMovementImmediately();
	}
}