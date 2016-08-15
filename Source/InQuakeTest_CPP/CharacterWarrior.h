// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "DM_GameState.h"
#include "FakeProjectile.h"
#include "RealProjectile.h"
#include "CharacterWarrior.generated.h"

UCLASS()
class INQUAKETEST_CPP_API ACharacterWarrior : public ACharacter
{
	GENERATED_BODY()

private:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	/** replicated test */
	UPROPERTY(Replicated)
		float Health;

	//health at spawn
	float MaxHealth;

	//instead of getting gamestate everytime we can just set it on beginplay, then get it from here after checking IsValid()
	TWeakObjectPtr<ADM_GameState> GameState;

	//not replicated, evaluated separately, sometimes synchronises.
	float MeeleeCooldown;

	//not replicated, evaluated separately, sometimes synchronises.
	float ShootCooldown;

	//if attack should be launched as soon as cooldown ends (player pressed button a little bit early).
	//Separated on client and s	erver.
	bool DefferedAttack;

	//if shooting should be launched as soon as cooldown ends (player pressed button a little bit early).
	//only works on server
	bool DefferedShoot;

	//Target for deffered shoot (on server)
	FVector DefferedTarget;

	//FIFO queue of fake projectiles, that are not yet synced with real ones
	TQueue<TWeakObjectPtr<AFakeProjectile>> FreeFakeProjectiles;

public:
	/** Set attack animation montage in blueprints */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UAnimMontage* AttackAnim;

	/** Set shoot animation montage in blueprints */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UAnimMontage* ShootAnim;

	/** Blueprint for fake projectile*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TSubclassOf<AFakeProjectile> FakeProjectileClass;

	/** Blueprint for real projectile*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TSubclassOf<ARealProjectile> RealProjectileClass;

	/** get current health*/
	UFUNCTION(BlueprintCallable, Category=CharacterWarrior)
		float GetHealth();

	/** get maximum health*/
	UFUNCTION(BlueprintCallable, Category=CharacterWarrior)
		float GetMaxHealth();

	/** get current meelee cooldown*/
	UFUNCTION(BlueprintCallable, Category=CharacterWarrior)
		float GetMeeleeCooldown();

	/** get current shooting cooldown*/
	UFUNCTION(BlueprintCallable, Category=CharacterWarrior)
		float GetShootCooldown();
protected:
	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** Called on client when player presses attack button */
	void Client_TryAttack();

	/** Called on client when player presses attack button */
	void Client_TryShoot();

	/** Show attack animation on client */
	void Client_Attack();

	/** Show shooting animation on client */
	void Client_Shoot(const FVector& TargetPoint);

public:
	ACharacterWarrior();
	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	//taking damage, both from meelee and shooting
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

	//Actual attack, called on server from TryAttack if character can attack
	void Server_Attack();

	//Actual shooting, called on server from TryShoot if character can shoot
	void Server_Shoot(const FVector& TargetPoint);

	//return first fake projectile without its real counterpart or nullptr if list is empty
	TWeakObjectPtr<AFakeProjectile> PopFakeProjectile();

	/** Called on server when player presses attack */
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_TryAttack();

	/** Sync meelee cooldown on client to server */
	UFUNCTION(Client, Reliable)
		void Client_SyncMeeleeCooldown(float NewCooldown);

	/** Called on server when player presses shoot */
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_TryShoot(const FVector& TargetPoint);

	/** Sync shoot cooldown on client to server */
	UFUNCTION(Client, Reliable)
		void Client_SyncShootCooldown(float NewCooldown);
	
	/** Sync shoot cooldown on client to server */
	UFUNCTION(NetMulticast, Reliable)
		void Client_Die();
};
