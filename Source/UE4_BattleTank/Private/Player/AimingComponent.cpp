// Copyright 2018 Stuart McDonald.

#include "AimingComponent.h"
#include "Tank.h"
#include "TankPlayerController.h"
#include "AIBot/TankAIController.h"
#include "Turret.h"
#include "Barrel.h"
#include "Projectile.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

UAimingComponent::UAimingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	CurrentFiringState = EFiringState::Aiming;
	bWantsToFire = false;
	bIsLoaded = false;
	bPendingReload = false;
}

void UAimingComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentRoundsRemaining = WeaponData.MaxRounds;
	if (!bIsLoaded)
	{
		ReloadProjectile();
	}
}

void UAimingComponent::Initialise(UBarrel * BarrelToSet, UTurret * TurretToSet, ATank * NewOwner)
{
	Barrel = BarrelToSet;
	Turret = TurretToSet;
	CompOwner = NewOwner;
}


////////////////////////////////////////////////////////////////////////////////
// Input

void UAimingComponent::AimAt(FVector HitLocation)
{
	if (!ensure(Barrel)) { return; }

	FVector OutLaunchVelocity;
	FVector StartLocation = Barrel->GetSocketLocation(FName("Projectile"));
	bool bHaveAimSolution = UGameplayStatics::SuggestProjectileVelocity(
		this,
		OutLaunchVelocity,
		StartLocation,
		HitLocation,
		WeaponData.LaunchSpeed,
		false,
		0,
		0,
		ESuggestProjVelocityTraceOption::DoNotTrace
	);

	if (bHaveAimSolution)
	{
		AimDirection = OutLaunchVelocity.GetSafeNormal();
		MoveBarrelTowards(AimDirection);
	}

	// If no solution.
}

void UAimingComponent::MoveBarrelTowards(FVector Direction)
{
	if (!ensure(Barrel)) { return; }
	// Workout difference between barrell location and aim direction.
	auto BarrelRotator = Barrel->GetForwardVector().Rotation();
	auto AimAsRotator = Direction.Rotation();
	auto DeltaRotator = AimAsRotator - BarrelRotator;

	// Elevate Barrel.
	Barrel->ElevateBarrel(DeltaRotator.Pitch);

	// Always yaw the shortest way
	if (FMath::Abs(DeltaRotator.Yaw) < 180)
	{
		Turret->RotateTurret(DeltaRotator.Yaw);
	}
	else // Avoid going the long-way round
	{
		Turret->RotateTurret(-DeltaRotator.Yaw);
	}
}

void UAimingComponent::FireProjectile()
{
	if (CanFire() && !bWantsToFire)
	{
		bWantsToFire = true;
		DetermineWeaponState();
	}
	else if (CurrentFiringState == EFiringState::OutOfAmmo)
	{
		UpdatePlayerHud();
	}
}


////////////////////////////////////////////////////////////////////////////////
// Control

void UAimingComponent::DetermineWeaponState()
{
	EFiringState FireState = EFiringState::Aiming;

	if (!bIsLoaded)
	{
		if (CurrentRoundsRemaining <= 0)
		{
			FireState = EFiringState::OutOfAmmo;
		}
		else
		{
			FireState = EFiringState::Reloading;
		}
	}
	else if (bIsLoaded)
	{
		if (bWantsToFire && CanFire())
		{
			FireState = EFiringState::Firing;
		}
	}

	SetWeaponState(FireState);
}

void UAimingComponent::SetWeaponState(EFiringState State)
{
	CurrentFiringState = State;

	if (State == EFiringState::Firing)
	{
		OnFire();
	}
}

FVector UAimingComponent::GetAdjustedAim() const
{
	ATankPlayerController * const PC = CompOwner ? Cast<ATankPlayerController>(CompOwner->Controller) : nullptr;
	FVector FinalAim = FVector::ZeroVector;
	// If we have a player controller use it for the aim
	if (PC)
	{
		FinalAim = (PC->GetCrosshairLocation() - Barrel->GetSocketLocation(FName("Projectile"))).GetSafeNormal();
	}
	else if (CompOwner)
	{
		FinalAim = Barrel->GetSocketRotation(FName("Projectile")).Vector();
	}

	return FinalAim;
}


////////////////////////////////////////////////////////////////////////////////
// Weapon Usage

void UAimingComponent::OnFire()
{
	if (CanFire())
	{
		// Spawn a projectile at the socket location on the barrel.
		if (!ensure(Barrel)) { return; }

		FTransform SpawnT(GetAdjustedAim().Rotation(), Barrel->GetSocketLocation(FName("Projectile")));

		SpawnParticleEffect(FireFX);

		AProjectile * Projectile = Cast<AProjectile>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, WeaponData.ProjectileBlueprint, SpawnT));
		if (Projectile && CompOwner)
		{
			Projectile->Instigator = CompOwner;
			Projectile->SetOwner(CompOwner);

			UGameplayStatics::FinishSpawningActor(Projectile, SpawnT);
			Projectile->LaunchProjectile(WeaponData.LaunchSpeed);
			PlayFireSound(FireSound);
		}

		ReloadProjectile();
	}
}

void UAimingComponent::ReloadProjectile()
{
	bWantsToFire = false;
	bIsLoaded = false;
	DetermineWeaponState();
	UpdatePlayerHud();

	if (CurrentFiringState == EFiringState::Reloading)
	{
		GetWorld()->GetTimerManager().SetTimer(ReloadTimer, this, &UAimingComponent::OnReloadFinished, WeaponData.ReloadTimeInSeconds, false);
	}
}

void UAimingComponent::OnReloadFinished()
{
	GetWorld()->GetTimerManager().ClearTimer(ReloadTimer);
	if (!WeaponData.bInfiniteAmmo)
	{
		CurrentRoundsRemaining--;
	}
	bIsLoaded = true;
	DetermineWeaponState();
	UpdatePlayerHud();
}

void UAimingComponent::CollectAmmo(int32 AmmoToAdd)
{
	int32 AmmoUsed = FMath::Max(0, WeaponData.MaxRounds - CurrentRoundsRemaining);

	AmmoToAdd = FMath::Min(AmmoToAdd, AmmoUsed);

	CurrentRoundsRemaining += AmmoToAdd;
	UpdatePlayerHud();

	if (!bIsLoaded)
	{
		ReloadProjectile();
	}
}


////////////////////////////////////////////////////////////////////////////////
// FX

UAudioComponent * UAimingComponent::PlayFireSound(USoundBase * Sound)
{
	ATankPlayerController * const PC = CompOwner ? Cast<ATankPlayerController>(CompOwner->Controller) : nullptr;
	if (PC)
	{
		PC->ClientPlayCameraShake(FireCamShakeBP);
	}

	UAudioComponent * AC = nullptr;
	if (Sound && CompOwner)
	{
		AC = UGameplayStatics::SpawnSoundAttached(Sound, CompOwner->GetRootComponent());
	}

	return AC;
}

void UAimingComponent::SpawnParticleEffect(UParticleSystem * Particle)
{
	if (Particle && Barrel)
	{
		ParticleComp = UGameplayStatics::SpawnEmitterAttached(Particle, Barrel, "Projectile");
	}
}


////////////////////////////////////////////////////////////////////////////////
// Reading Data

bool UAimingComponent::IsBarrelMoving()
{
	if (!ensure(Barrel)) { return false; }
	auto BarrelForward = Barrel->GetForwardVector();
	return !BarrelForward.Equals(AimDirection, 0.01);
}

bool UAimingComponent::CanFire() const
{
	bool bCanFire = CurrentFiringState != EFiringState::Reloading || CurrentFiringState != EFiringState::OutOfAmmo;
	return bCanFire && bIsLoaded;
}

void UAimingComponent::UpdatePlayerHud()
{
	APawn * Owner = CompOwner ? CompOwner : Cast<APawn>(GetOwner());
	ATankPlayerController * PC = Cast<ATankPlayerController>(Owner->GetController());
	if (PC)
	{
		PC->UpdateFiringStateDisplay();
		CurrentFiringState == EFiringState::OutOfAmmo ? PC->WarnOutOfAmmo(true) : PC->WarnOutOfAmmo(false);
		CurrentFiringState != EFiringState::OutOfAmmo && CurrentRoundsRemaining  <= 10 ? PC->WarnOfLowAmmo(true) : PC->WarnOfLowAmmo(false);
	}
}

