// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerState.h"
#include "DM_PlayerState.generated.h"

/**
 * 
 */
UCLASS()
class INQUAKETEST_CPP_API ADM_PlayerState : public APlayerState
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(Replicated)
		int32 Frags;
	
public:
	ADM_PlayerState();

	int32 GetFrags();

	int32 AddFrag();
};
