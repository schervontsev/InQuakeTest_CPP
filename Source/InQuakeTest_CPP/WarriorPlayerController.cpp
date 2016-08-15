// Fill out your copyright notice in the Description page of Project Settings.

#include "InQuakeTest_CPP.h"
#include "WarriorPlayerController.h"
#include "CharacterWarrior.h"

void AWarriorPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (!HasAuthority()) {
		if (HUDTemplate) {
			if (!WarriorHUD) {
				WarriorHUD = CreateWidget<UUserWidget>(this, HUDTemplate);
			}
		}
		if (!WarriorHUD->GetIsVisible()) {
			WarriorHUD->AddToViewport();
		}
	}
}
