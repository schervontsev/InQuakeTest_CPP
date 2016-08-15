// Fill out your copyright notice in the Description page of Project Settings.

#include "InQuakeTest_CPP.h"
#include "DM_PlayerState.h"
#include "UnrealNetwork.h"

ADM_PlayerState::ADM_PlayerState()
	: Frags(0)
{
	//replication defaults
	bReplicates = true;
	bAlwaysRelevant = true;
}

int32 ADM_PlayerState::GetFrags()
{
	return Frags;
}

int32 ADM_PlayerState::AddFrag()
{
	check(HasAuthority());
	return ++Frags;
}

void ADM_PlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADM_PlayerState, Frags);
}