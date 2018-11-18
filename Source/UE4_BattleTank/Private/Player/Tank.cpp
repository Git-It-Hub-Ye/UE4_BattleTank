// Copyright 2018 Stuart McDonald.

#include "Tank.h"
#include "TankPlayerController.h"
#include "Components/SkeletalMeshComponent.h"
#include "AimingComponent.h"

ATank::ATank()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	TankBody = CreateDefaultSubobject<USkeletalMeshComponent>(FName("TankBody"));
	SetRootComponent(TankBody);

	// Will set physics after spawn in begin play to avoid teleport flag errors.
	TankBody->SetSimulatePhysics(false);

	AimingComp = CreateDefaultSubobject<UAimingComponent>(FName("AimComponent"));

	bHasBeenDestroyed = false;
}

void ATank::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = StartingHealth;
	TankBody->SetSimulatePhysics(true);
}

void ATank::SetupPlayerInputComponent(UInputComponent * PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAction("Fire", IE_Pressed, this, &ATank::Fire);
}

void ATank::Fire()
{
	if (AimingComp)
	{
		AimingComp->FireProjectile();
	}
}

float ATank::TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser)
{
	int32 DamagePoints = FPlatformMath::RoundToInt(DamageAmount);
	int32 DamageToApply = FMath::Clamp(DamagePoints, 0, CurrentHealth);

	CurrentHealth -= DamageToApply;

	if (CurrentHealth <= 0)
	{
		bHasBeenDestroyed = true;
		OnDeath.Broadcast();
	}

	ATankPlayerController * const PC = this->Controller ? Cast<ATankPlayerController>(this->Controller) : nullptr;
	if (PC)
	{
		PC->ClientPlayCameraShake(DamageCamShakeBP);
	}

	return DamageToApply;
}

float ATank::GetHealthPercent() const
{
	return (float)CurrentHealth;
}

void ATank::ReplenishHealth(float HealthToAdd)
{
	CurrentHealth = CurrentHealth + HealthToAdd;
	CurrentHealth = FMath::Clamp<int32>(CurrentHealth, 0, StartingHealth);
}

