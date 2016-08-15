// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "WarriorHUD.generated.h"

/**
 * 
 */
UCLASS()
class INQUAKETEST_CPP_API UWarriorHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/** Text for healthbar */
	UFUNCTION(BlueprintCallable, Category=WarriorHUD)
		FText GetHPText();

	/** Percentage for healthbar */
	UFUNCTION(BlueprintCallable, Category=WarriorHUD)
		float GetHPPercent();

	/** Percentage for meelee cooldown */
	UFUNCTION(BlueprintCallable, Category=WarriorHUD)
		float GetMeeleePercent();

	/** Percentage for shooting cooldown */
	UFUNCTION(BlueprintCallable, Category=WarriorHUD)
		float GetShootPercent();

	/** Text for my frags */
	UFUNCTION(BlueprintCallable, Category=WarriorHUD)
		FText GetMyFragsText();


	/** Text for best player's frags */
	UFUNCTION(BlueprintCallable, Category=WarriorHUD)
		FText GetBestFragsText();

	/** Text for max match frags */
	UFUNCTION(BlueprintCallable, Category=WarriorHUD)
		FText GetMaxFragsText();
};
