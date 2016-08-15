// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BaseProjectile.h"
#include "FakeProjectile.h"
#include "RealProjectile.generated.h"

/**
 * 
 */
UCLASS()
class INQUAKETEST_CPP_API ARealProjectile : public ABaseProjectile
{
	GENERATED_BODY()
	
private:
	//fake counterpart (only exists on owner's client)
	TWeakObjectPtr<AFakeProjectile> FakeProjectile;

	//set from GameState consts after spawning
	float DamageAmount;
protected:
	//time before syncing with fake counterpart
	FTimerHandle SyncTimer;
public:
	ARealProjectile();
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// overlap
	virtual void NotifyActorBeginOverlap(AActor * OtherActor) override;

	void SetDamageAmount(float GotDamageAmount);

	/**Stops real projectile on client*/
	UFUNCTION(NetMulticast, Unreliable)
		void Client_StopProjectile();

private:
	void SyncFake();
};
