// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "WarriorHUD.h"
#include "WarriorPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class INQUAKETEST_CPP_API AWarriorPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = UI)
		TSubclassOf<UWarriorHUD> HUDTemplate;

	UPROPERTY()
		UUserWidget* WarriorHUD;
	
public:
	virtual void BeginPlay() override;
	
	void RespawnPlayer();
};
