// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AimingComponent.generated.h"

// Forward Declerations.
class UBarrel;
class UTurret;
class ATank;
class AProjectile;
class UAudioComponent;
class UParticleSystemComponent;
class UCameraShake;
class USoundBase;

// Enum for Aiming state.
UENUM()
enum class EFiringState : uint8 {
	Aiming,
	Reloading,
	OutOfAmmo,
	Firing
};

// Struct for weapon data
USTRUCT(BlueprintType)
struct FWeaponData {
	GENERATED_USTRUCT_BODY()

	/** Projectile fired from vehicle */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AProjectile> ProjectileBlueprint;

	/** True if component has infinite supply of ammo, (Used for AI owners) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	bool bInfiniteAmmo;

	/** Max rounds able to be loaded */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (ClampMin = 1, ClampMax = 100))
	int32 MaxRounds;

	/** Speed projectile is fired */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (ClampMin = 1000, ClampMax = 20000))
	float LaunchSpeed;

	/** Time to reload projectile */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (ClampMin = 0.1, ClampMax = 10))
	float ReloadTimeInSeconds;

	/** Defaults */
	FWeaponData()
	{
		bInfiniteAmmo = false;
		MaxRounds = 20.f;
		LaunchSpeed = 4000.f;
		ReloadTimeInSeconds = 3.f;
	}
};

/**
* Ability to control and turret and barrel
*/

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE4_BATTLETANK_API UAimingComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	/** Current state of component */
	EFiringState CurrentFiringState;

	/** Component weapon data */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FWeaponData WeaponData;

private:
	/** Barrel of component for elevation */
	UBarrel * Barrel = nullptr;

	/** Turret of component for rotation */
	UTurret * Turret = nullptr;

	/** Owner */
	ATank * CompOwner;

	/** Current Rounds remaining */
	int32 CurrentRoundsRemaining;

	/** Owner want to fire */
	bool bWantsToFire;

	/** Owner has reloaded */
	bool bIsLoaded;

	/** Owner need to reload */
	bool bPendingReload;

	/** Direction component is aiming */
	FVector AimDirection;

	/** Timer handle for reload */
	FTimerHandle ReloadTimer;


	////////////////////////////////////////////////////////////////////////////////
	// FX

	/** Camera shake on firing */
	UPROPERTY(EditDefaultsOnly, Category = "FX")
	TSubclassOf<UCameraShake> FireCamShakeBP;

	/** Component for fx on fp mesh */
	UParticleSystemComponent * ParticleComp;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UParticleSystem * FireFX;

	/** Fire sound fx */
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase * FireSound;

	/** Load sound fx */
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase * LoadSound;

public:
	// Sets default values for this component's properties
	UAimingComponent();

	/** Initialise the barrel and turret from owner */
	UFUNCTION(BlueprintCallable, Category = "Setup")
	void Initialise(UBarrel * BarrelToSet, UTurret * TurretToSet, ATank * NewOwner);


	////////////////////////////////////////////////////////////////////////////////
	// Input

	/** Get location to aim component towards */
	void AimAt(FVector HitLocation);

	/** Move barrel and turret towards aim location */
	void MoveBarrelTowards(FVector Direction);

	/** Fire projectile */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void FireProjectile();


	////////////////////////////////////////////////////////////////////////////////
	// Reading Data

	/** Is barrel and turret moving */
	bool IsBarrelMoving();

	/** Return current firing state */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	EFiringState GetFiringState() const { return CurrentFiringState; }

	/** Rounds remaing */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	int32 GetRoundsRemaining() const { return CurrentRoundsRemaining; }

	/** Is current round less than max rounds */
	bool NeedAmmo() { return CurrentRoundsRemaining < WeaponData.MaxRounds; }

	/** Return the difference between current and max rounds */
	int32 GetAmmoDifference() { return WeaponData.MaxRounds - CurrentRoundsRemaining; }

	/** Add ammo */
	void CollectAmmo(int32 AmmoToAdd);

protected:
	virtual void BeginPlay() override;

	////////////////////////////////////////////////////////////////////////////////
	// Weapon usage

	/** Try fire a projectile */
	void OnFire();

	/** Set reload timer and component state */
	void ReloadProjectile();

	/** Adjust current ammo and component state  */
	void OnReloadFinished();

private:
	////////////////////////////////////////////////////////////////////////////////
	// Control

	/** Determines what state component is in */
	void DetermineWeaponState();

	/** Sets the current state of component */
	void SetWeaponState(EFiringState State);

	/** Get aim direction & rotation depending on controller */
	FVector GetAdjustedAim() const;


	////////////////////////////////////////////////////////////////////////////////
	// FX

	/** Play sounds on fire */
	UFUNCTION()
	UAudioComponent * PlaySoundFX(USoundBase * Sound);

	/** Play particle FX */
	void SpawnParticleEffect(UParticleSystem * Particle);


	////////////////////////////////////////////////////////////////////////////////
	// Reading Data

	/** Check if component is in correct state to fire */
	bool CanFire() const;

	/** Notify player controller to update player ui */
	void UpdatePlayerHud();
	
};
