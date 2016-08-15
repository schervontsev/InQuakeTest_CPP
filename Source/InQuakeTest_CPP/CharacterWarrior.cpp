// Fill out your copyright notice in the Description page of Project Settings.

#include "InQuakeTest_CPP.h"
#include "CharacterWarrior.h"
#include "UnrealNetwork.h"
#include "DM_PlayerState.h"
#include "WarriorPlayerController.h"

ACharacterWarrior::ACharacterWarrior()
	: DefferedAttack(false)
	, DefferedShoot(false)
	, MeeleeCooldown(0.f)
	, ShootCooldown(0.f)
	, MaxHealth(100.f)
	, Health(MaxHealth)
	, GameState(nullptr)
{
 	// Set this character to call Tick() every frame.
	PrimaryActorTick.bCanEverTick = true;

	//replication defaults
	bReplicates = true;
	bNetLoadOnClient = true;
	bReplicateMovement = true;
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);

	// Our character should rotate by yaw with camera
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->AttachTo(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}

void ACharacterWarrior::BeginPlay()
{
	Super::BeginPlay();
	GameState = GetWorld() ? GetWorld()->GetGameState<ADM_GameState>() : nullptr;
	check(GameState.IsValid());
	if (HasAuthority()) {
		MaxHealth = GameState.IsValid() ? GameState->MaxHealth : 0.f;
		Health = MaxHealth;
	}
}

void ACharacterWarrior::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	if (MeeleeCooldown > 0.f) {
		MeeleeCooldown -= DeltaTime;
		if (MeeleeCooldown <= 0.f) {
			if (DefferedAttack) {
				if (HasAuthority()) {
					Server_Attack();
				} else {
					Client_Attack();
				}
			}
		}
	}
	if (ShootCooldown > 0.f) {
		ShootCooldown -= DeltaTime;
		if (ShootCooldown <= 0.f) {
			if (HasAuthority()) {
				if (DefferedShoot) {
					Server_Shoot(DefferedTarget);
				}
			}
		}
	}
}

void ACharacterWarrior::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	check(InputComponent);
	InputComponent->BindAxis("MoveForward", this, &ACharacterWarrior::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ACharacterWarrior::MoveRight);

	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	InputComponent->BindAction("Attack", IE_Pressed, this, &ACharacterWarrior::Client_TryAttack);
	InputComponent->BindAction("Shoot", IE_Pressed, this, &ACharacterWarrior::Client_TryShoot);
}

void ACharacterWarrior::Server_Attack()
{
	if (!HasAuthority()) {
		check(false);
		return;
	}

	DefferedAttack = false;
	MeeleeCooldown = GameState.IsValid() ? GameState->DefaultMeeleeCooldown : 0.f;
	//decrease cooldown sent to client by ping
	float Latency = 0.f;
	AController* Controller = GetController();
	if (Controller) {
		APlayerState* state = Controller->PlayerState;
		if (state) {
			Latency = 0.004f * Controller->PlayerState->Ping; //ping stored in 1/4 of real ping, convert to seconds
		}
	}
	Client_SyncMeeleeCooldown(MeeleeCooldown - Latency);

	//find players that will be damaged by this attack
	for (TActorIterator<ACharacterWarrior> Itr(GetWorld()); Itr; ++Itr) {
		if (*Itr != this) {
			//check if meelee attack reaches found player
			const float Reach = GameState.IsValid() ? GameState->MeeleeReach : 0.f;
			const float MaxAngle = GameState.IsValid() ? GameState->MeeleeAngle : 0.f;
			FVector StrikeVector = Itr->GetActorLocation() - GetActorLocation();
			StrikeVector.Normalize();
			const float Distance = FVector::Dist(GetActorLocation(), Itr->GetActorLocation()) - GetCapsuleComponent()->GetScaledCapsuleRadius() * 2.f;
			const float Angle = FMath::Abs(FMath::RadiansToDegrees(acosf(FVector::DotProduct(GetActorForwardVector(), StrikeVector))));
			if (Distance <= Reach) {
				if (Angle <= MaxAngle) {
					FDamageEvent DamageEvent;
					Itr->TakeDamage(GameState.IsValid() ? GameState->MeeleeDamage : 0.f, DamageEvent, GetController(), this);
				}
			}
		}
	}
}

void ACharacterWarrior::Server_Shoot(const FVector & TargetPoint)
{
	DefferedShoot = false;
	ShootCooldown = GameState.IsValid() ? GameState->DefaultShootCooldown : 0.f;
	//decrease cooldown sent to client by ping
	float Latency = 0.f;
	AController* controller = GetController();
	if (controller) {
		APlayerState* state = controller->PlayerState;
		if (state) {
			Latency = 0.002 * controller->PlayerState->Ping; //ping stored in 1/4 of real ping, convert to seconds
		}
	}
	Client_SyncShootCooldown(ShootCooldown - Latency);
	if (RealProjectileClass) {
		FActorSpawnParameters SpawnParam = FActorSpawnParameters();
		SpawnParam.Instigator = this;
		FVector ShootPoint = GetActorLocation();
		FRotator Rotation = FRotationMatrix::MakeFromX(TargetPoint - ShootPoint).Rotator();
		AActor* SpawnedActor = GetWorld()->SpawnActor(RealProjectileClass, &ShootPoint, &Rotation, SpawnParam);
		TWeakObjectPtr<ARealProjectile> Projectile = Cast<ARealProjectile>(SpawnedActor);
		if (Projectile.IsValid()) {
			Projectile->SetDamageAmount(GameState.IsValid() ? GameState->ShootDamage : 0.f);
		} else {
			check(false);
		}
	} else {
		check(false);
	}
}

TWeakObjectPtr<AFakeProjectile> ACharacterWarrior::PopFakeProjectile()
{
	TWeakObjectPtr<AFakeProjectile> Result;
	if (!FreeFakeProjectiles.Dequeue(Result)) {
		return nullptr;
	} else {
		return Result;
	}
}

void ACharacterWarrior::Client_Die_Implementation()
{

}

float ACharacterWarrior::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	check(HasAuthority());
	Health -= DamageAmount;
	if (Health <= 0.f) {
		Health = 0.f;
		ACharacterWarrior* OtherPlayer = Cast<ACharacterWarrior>(DamageCauser);
		if (OtherPlayer) {
			AWarriorPlayerController* InstigatorController = Cast<AWarriorPlayerController>(OtherPlayer->GetController());
			if (InstigatorController) {
				ADM_PlayerState* State = Cast<ADM_PlayerState>(InstigatorController->PlayerState);
				if (State && GameState.IsValid()) {
					GameState->UpdateMaxFrags(State->AddFrag());
				} else {
					check(false);
				}
			} else {
				check(false);
			}
		} else {
			check(false);
		}
		

		//quick and dirty "respawn"
		Health = MaxHealth;
		MeeleeCooldown = (GameState.IsValid() ? GameState->DefaultMeeleeCooldown : 0.f);
		ShootCooldown = (GameState.IsValid() ? GameState->DefaultShootCooldown : 0.f);
		TArray<AActor*> Starts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), Starts);
		SetActorTransform(Starts[FMath::RandRange(0, Starts.Num() - 1)]->GetTransform());
	}
	return DamageAmount;
}

void ACharacterWarrior::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACharacterWarrior, Health);
}

void ACharacterWarrior::Server_TryAttack_Implementation()
{
	if (DefferedAttack) {
		return;
	}
	if (MeeleeCooldown <= 0.f) {
		Server_Attack();
	} else {
		DefferedAttack = true;
	}
}

bool ACharacterWarrior::Server_TryAttack_Validate()
{
	//we don't want to disconnect players here, not enough information to separate cheaters from laggers
	return true;
}


void ACharacterWarrior::Server_TryShoot_Implementation(const FVector& TargetPoint)
{
	if (DefferedShoot) {
		return;
	}
	if (ShootCooldown <= 0.f) {
		Server_Shoot(TargetPoint);
	} else {
		DefferedShoot = true;
		DefferedTarget = TargetPoint;
	}
}

bool ACharacterWarrior::Server_TryShoot_Validate(const FVector& TargetPoint)
{
	//we don't want to disconnect players here, not enough information to separate cheaters from laggers
	return true;
}

void ACharacterWarrior::Client_SyncMeeleeCooldown_Implementation(float NewCooldown)
{
	check(Role == ROLE_AutonomousProxy);
	MeeleeCooldown = NewCooldown;
}

void ACharacterWarrior::Client_SyncShootCooldown_Implementation(float NewCooldown)
{
	check(Role == ROLE_AutonomousProxy);
	ShootCooldown = NewCooldown;
}

float ACharacterWarrior::GetHealth()
{
	return Health;
}

float ACharacterWarrior::GetMaxHealth()
{
	return MaxHealth;
}

float ACharacterWarrior::GetMeeleeCooldown()
{
	return MeeleeCooldown;
}

float ACharacterWarrior::GetShootCooldown()
{
	return ShootCooldown;
}

void ACharacterWarrior::MoveForward(float Value)
{
	check(Controller);
	if (Controller && Value != 0.f) {
		// get forward vector
		const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ACharacterWarrior::MoveRight(float Value)
{
	check(Controller);
	if (Controller && Value != 0.f) {
		// get right vector 
		const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void ACharacterWarrior::Client_TryAttack()
{
	check(!HasAuthority());
	if (MeeleeCooldown <= 0.f) {
		Server_TryAttack();
		Client_Attack();
	} else if (MeeleeCooldown <= (GameState.IsValid() ? GameState->DefferedTime : 0.f)) {
		Server_TryAttack();
		DefferedAttack = true;
	}
}

void ACharacterWarrior::Client_TryShoot()
{
	check(!HasAuthority());
	if (ShootCooldown <= 0.f) {
		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		if (PlayerController) {
			//tracing aim to target
			FVector Target;
			int32 ScreenX;
			int32 ScreenY;
			PlayerController->GetViewportSize(ScreenX, ScreenY);
			FVector TraceStart;
			FVector TraceDirection;
			PlayerController->DeprojectScreenPositionToWorld(ScreenX / 2, ScreenY / 2, TraceStart, TraceDirection);
			FHitResult OutHit;
			const FVector TraceEnd = TraceStart + TraceDirection * 5000.f;
			if (GetWorld()->LineTraceSingleByChannel(OutHit, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility)) {
				Target = OutHit.Location;
			} else {
				Target = TraceEnd;
			}
			Server_TryShoot(Target);
			Client_Shoot(Target);
		} else {
			check(false);
		}
	}
	//no deffered attack for shooting on client
}

void ACharacterWarrior::Client_Attack()
{
	DefferedAttack = false;
	MeeleeCooldown = (GameState.IsValid() ? GameState->DefaultMeeleeCooldown : 0.f);
	if (AttackAnim) {
		PlayAnimMontage(AttackAnim);
	} else {
		check(false);
	}
}

void ACharacterWarrior::Client_Shoot(const FVector& TargetPoint)
{
	if (!FakeProjectileClass) {
		check(false);
	} else {
		FActorSpawnParameters SpawnParam = FActorSpawnParameters();
		SpawnParam.Instigator = this;
		FVector ShootPoint = GetActorLocation();
		FRotator Rotation = FRotationMatrix::MakeFromX(TargetPoint - ShootPoint).Rotator();
		AActor* SpawnedActor = GetWorld()->SpawnActor(FakeProjectileClass, &ShootPoint, &Rotation, SpawnParam);
		TWeakObjectPtr<AFakeProjectile> Ptr = Cast<AFakeProjectile>(SpawnedActor);
		check(Ptr.IsValid());
		FreeFakeProjectiles.Enqueue(Ptr);
	}
	ShootCooldown = (GameState.IsValid() ? GameState->DefaultShootCooldown : 0.f);
	if (ShootAnim) {
		PlayAnimMontage(ShootAnim);
	} else {
		check(false);
	}
}

