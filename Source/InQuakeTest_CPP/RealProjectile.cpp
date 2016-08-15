// Fill out your copyright notice in the Description page of Project Settings.

#include "InQuakeTest_CPP.h"
#include "RealProjectile.h"
#include "CharacterWarrior.h"

ARealProjectile::ARealProjectile(): ABaseProjectile()
	, FakeProjectile(nullptr)
	, DamageAmount(0.f)
{
	//replication defaults
	bReplicates = true;
	bNetLoadOnClient = true;
	bReplicateMovement = true;
}

void ARealProjectile::BeginPlay()
{
	Super::BeginPlay();
	AController* InstigatorController = GetInstigatorController();
	if (InstigatorController && InstigatorController->IsLocalPlayerController()) {
		SetActorHiddenInGame(true);
		//We will try to sync projectiles depending on ping
		float Latency = 0.f;
		APlayerState* state = InstigatorController->PlayerState;
		if (state) {
			Latency = 0.002f * InstigatorController->PlayerState->Ping; //ping stored in 1/4 of real ping, take half and convert to seconds
		}
		GetWorld()->GetTimerManager().SetTimer(SyncTimer, this, &ARealProjectile::SyncFake, Latency);
		//Get fake counterpart from queue of free fake projectiles
		if (Instigator) {
			ACharacterWarrior* InstigatorPlayer = Cast<ACharacterWarrior>(Instigator);
			if (InstigatorPlayer) {
				FakeProjectile = InstigatorPlayer->PopFakeProjectile();
			}
		}
	}
}

void ARealProjectile::NotifyActorBeginOverlap(AActor * OtherActor)
{
	if (OtherActor != GetInstigator()) {
		if (!HasAuthority()) {
			//sync with fake counterpart on owning client (we don't check for ownership, because it's easier to check for FakeProjectile validity)
			SyncFake();
		} else {
			MovementComp->StopMovementImmediately();
			Client_StopProjectile();
			SetLifeSpan(2.f);
			ACharacterWarrior* OtherPlayer = Cast<ACharacterWarrior>(OtherActor);
			if (OtherPlayer) {
				FDamageEvent DamageEvent;
				OtherPlayer->TakeDamage(DamageAmount, DamageEvent, GetInstigatorController(), GetInstigator());
				AttachRootComponentToActor(OtherActor, FName("Name_None"), EAttachLocation::KeepWorldPosition);
				SetActorEnableCollision(false);
			}
		}
	}
}

void ARealProjectile::SetDamageAmount(float GotDamageAmount)
{
	DamageAmount = GotDamageAmount;
}

void ARealProjectile::Client_StopProjectile_Implementation()
{
	MovementComp->StopMovementImmediately();
	
}

void ARealProjectile::SyncFake()
{
	if (FakeProjectile.IsValid()) {
		FakeProjectile->Destroy();
		SetActorHiddenInGame(false);
	}
	FakeProjectile = nullptr;
}