// Fill out your copyright notice in the Description page of Project Settings.

#include "InQuakeTest_CPP.h"
#include "DM_GameState.h"
#include "UnrealNetwork.h"

ADM_GameState::ADM_GameState()
	: DefaultMeeleeCooldown(2.f)
	, DefaultShootCooldown(2.f)
	, DefferedTime(1.f)
	, MeeleeDamage(40.f)
	, ShootDamage(10.f)
	, MaxHealth(100.f)
	, MeeleeReach(100.f)
	, MeeleeAngle(45.f)
	, BestFrags(0)
	, MaxFrags(10)
{
	//replication defaults
	bReplicates = true;
	bAlwaysRelevant = true;
}

void ADM_GameState::UpdateMaxFrags(int32 NewFrags)
{
	check(HasAuthority());
	if (BestFrags < NewFrags) {
		BestFrags = NewFrags;
		if (BestFrags >= MaxFrags) {
			GetWorld()->ServerTravel(TEXT("/Game/Maps/StartMap?listen"), true);
		}
	}
}

void ADM_GameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADM_GameState, DefaultMeeleeCooldown);
	DOREPLIFETIME(ADM_GameState, DefaultShootCooldown);
	DOREPLIFETIME(ADM_GameState, DefferedTime);
	DOREPLIFETIME(ADM_GameState, BestFrags);
	DOREPLIFETIME(ADM_GameState, MaxFrags);
}
