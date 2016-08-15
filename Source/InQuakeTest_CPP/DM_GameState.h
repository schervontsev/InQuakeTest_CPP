// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameState.h"
#include "DM_GameState.generated.h"

/**
 * 
 */
UCLASS()
class INQUAKETEST_CPP_API ADM_GameState : public AGameState
{
	GENERATED_BODY()

public:
	/** Change meelee cooldown here */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
		float DefaultMeeleeCooldown;

	/** Change ranged cooldown here */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
		float DefaultShootCooldown;

	/** Time before cooldown end, when you can already press button and get response when timer is ended */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
		float DefferedTime;

	/** Default damage from meelee attacks */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float MeeleeDamage;

	/** Default damage from ranged attacks */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float ShootDamage;

	/** Default players HP */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
		float MaxHealth;

	/** Distance between two players' capsules for meelee attack */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float MeeleeReach;

	/** Max angle between player's forward vector and shortest path between player and victim */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float MeeleeAngle;

	/** Best frag score in game */
	UPROPERTY(Replicated)
		int32 BestFrags;

	/** Maximum frags in game */
	UPROPERTY(Replicated)
		int32 MaxFrags;

public:
	ADM_GameState();

	//check if new frags count is max
	void UpdateMaxFrags(int32 NewFrags);
};
