// Fill out your copyright notice in the Description page of Project Settings.

#include "InQuakeTest_CPP.h"
#include "WarriorHUD.h"
#include "CharacterWarrior.h"
#include "DM_PlayerState.h"

FText UWarriorHUD::GetHPText()
{
	ACharacterWarrior* Player = Cast<ACharacterWarrior>(GetOwningPlayerPawn());
	if (Player) {
		return FText::FromString(FString::FromInt(Player->GetHealth()));
	}
	return FText::FromString(FString(""));
}

float UWarriorHUD::GetHPPercent()
{
	ACharacterWarrior* Player = Cast<ACharacterWarrior>(GetOwningPlayerPawn());
	if (Player && Player->GetMaxHealth() != 0.f) {
		return Player->GetHealth() / Player->GetMaxHealth();
	}
	return 0.0f;
}

float UWarriorHUD::GetMeeleePercent()
{
	ACharacterWarrior* Player = Cast<ACharacterWarrior>(GetOwningPlayerPawn());
	if (Player) {
		ADM_GameState* GameState = Player->GetWorld() ? Player->GetWorld()->GetGameState<ADM_GameState>() : nullptr;
		if (GameState) {
			float DefCooldown = GameState->DefaultMeeleeCooldown;
			if (DefCooldown != 0.f) {
				return Player->GetMeeleeCooldown() / DefCooldown;
			}
		}
	}
	return 0.0f;
}

float UWarriorHUD::GetShootPercent()
{
	ACharacterWarrior* Player = Cast<ACharacterWarrior>(GetOwningPlayerPawn());
	if (Player) {
		ADM_GameState* GameState = Player->GetWorld() ? Player->GetWorld()->GetGameState<ADM_GameState>() : nullptr;
		if (GameState) {
			float DefCooldown = GameState->DefaultShootCooldown;
			if (DefCooldown != 0.f) {
				return Player->GetShootCooldown() / DefCooldown;
			}
		}
	}
	return 0.0f;
}

FText UWarriorHUD::GetMyFragsText()
{
	APlayerController* PlayerController = GetOwningPlayer();
	if (PlayerController) {
		ADM_PlayerState* PlayerState = Cast<ADM_PlayerState>(PlayerController->PlayerState);
		if (PlayerState) {
			return FText::FromString(FString::FromInt(PlayerState->GetFrags()));
		}
	}
	return FText::FromString(FString(""));
}

FText UWarriorHUD::GetBestFragsText()
{
	APlayerController* PlayerController = GetOwningPlayer();
	if (PlayerController) {
		ADM_GameState* GameState = Cast<ADM_GameState>(PlayerController->GetWorld() ? PlayerController->GetWorld()->GetGameState() : nullptr);
		if (GameState) {
			return FText::FromString(FString::FromInt(GameState->BestFrags));
		}
	}
	return FText::FromString(FString(""));
}


FText UWarriorHUD::GetMaxFragsText()
{
	APlayerController* PlayerController = GetOwningPlayer();
	if (PlayerController) {
		ADM_GameState* GameState = Cast<ADM_GameState>(PlayerController->GetWorld() ? PlayerController->GetWorld()->GetGameState() : nullptr);
		if (GameState) {
			return FText::FromString(FString::FromInt(GameState->MaxFrags));
		}
	}
	return FText::FromString(FString(""));
}