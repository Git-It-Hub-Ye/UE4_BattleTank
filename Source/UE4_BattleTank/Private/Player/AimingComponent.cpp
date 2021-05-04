// Copyright 2018 to 2021 Stuart McDonald.

#include "AimingComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

#include "Components/AudioComponent.h"
#include "TankPlayerController.h"
#include "AIBot/TankAIController.h"
#include "Tank.h"
#include "Turret.h"
#include "Barrel.h"
#include "Projectile.h"
#include "UI/BattleHUD.h"


UAimingComponent::UAimingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	CurrentFiringState = EFiringState::Aiming;
	bWantsToFire = false;
	bIsLoaded = false;
	bPendingReload = false;
	bLowAmmo = false;
	bAmmoWarning = false;
}

void UAimingComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentRoundsRemaining = WeaponData.MaxRounds;
}

void UAimingComponent::Initialise(UBarrel * BarrelToSet, UTurret * TurretToSet, APawn * NewOwner)
{
	Barrel = BarrelToSet;
	Turret = TurretToSet;
	CompOwner = NewOwner;

	if (!bIsLoaded)
	{
		ReloadProjectile();
	}

	if (GetOwner() != NULL)
	{
		ATank * OwnerPawn = Cast<ATank>(GetOwner());

		if (OwnerPawn == NULL) { return; }
		OwnerPawn->OnDeath.AddUniqueDynamic(this, &UAimingComponent::OnOwnerDeath);
	}
}

void UAimingComponent::OnOwnerDeath()
{
	StopActions();
}

void UAimingComponent::StopActions()
{
	GetWorld()->GetTimerManager().ClearTimer(ReloadTimer);
	if (AimCompAudio && AimCompAudio->IsPlaying())
	{
		AimCompAudio->Stop();
	}
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

		AProjectile * Projectile = Cast<AProjectile>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, WeaponData.ProjectileBlueprint, SpawnT));
		if (Projectile && CompOwner)
		{
			SpawnParticleEffect(FireFX);

			Projectile->Instigator = CompOwner;
			Projectile->SetOwner(CompOwner);

			UGameplayStatics::FinishSpawningActor(Projectile, SpawnT);
			Projectile->LaunchProjectile(WeaponData.LaunchSpeed);
			PlaySoundFX(FireSound);

			ATankPlayerController * const PC = CompOwner ? Cast<ATankPlayerController>(CompOwner->Controller) : nullptr;
			if (PC)
			{
				PC->ClientPlayCameraShake(FireCamShakeBP);
			}

			ReloadProjectile();
		}
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
		AimCompAudio = PlaySoundFX(LoadSound);
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

	if (CurrentFiringState == EFiringState::OutOfAmmo)
	{
		ReloadProjectile();
	}
	else
	{
		UpdatePlayerHud();
	}
}


////////////////////////////////////////////////////////////////////////////////
// FX

UAudioComponent * UAimingComponent::PlaySoundFX(USoundBase * Sound)
{
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
	ATankPlayerController * PC = CompOwner ? Cast<ATankPlayerController>(CompOwner->GetController()) : nullptr;
	ABattleHUD * BHUD = PC ? PC->GetPlayerHud() : nullptr;
	if (BHUD)
	{
		BHUD->UpdateWeaponStateDisplay();

		if (CurrentRoundsRemaining >= 12 && bAmmoWarning)
		{
			bLowAmmo = false;
			bAmmoWarning = false;
			BHUD->RemoveAmmoWarnings();
		}
		else if (CurrentFiringState != EFiringState::OutOfAmmo && CurrentRoundsRemaining < 12 && !bLowAmmo)
		{
			bLowAmmo = true;
			bAmmoWarning = true;
			BHUD->WarnOfLowAmmo();
		}
		else if (CurrentFiringState == EFiringState::OutOfAmmo)
		{
			bLowAmmo = false;
			bAmmoWarning = true;
			BHUD->WarnOutOfAmmo();
		}
	}
}

