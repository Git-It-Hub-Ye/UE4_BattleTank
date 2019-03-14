// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Tank.generated.h"

class UAimingComponent;
class UTankMovement;
class UCameraShake;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTankDelegate);

UCLASS()
class UE4_BATTLETANK_API ATank : public APawn
{
	GENERATED_BODY()

protected:
	/** Root mesh */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent * TankBody;

	/** Aim tank weapons */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UAimingComponent * AimingComp = nullptr;

	/** Move tank */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UTankMovement * MovementComp = nullptr;

private:
	/** Camera shake when damaged */
	UPROPERTY(EditDefaultsOnly, Category = "FX")
	TSubclassOf<UCameraShake> DamageCamShakeBP;

	/** Component for fx on actor */
	UParticleSystemComponent * ParticleComp;

	/** Destroy fx */
	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UParticleSystem * DestroyedFX;

	/** Starting health value */
	UPROPERTY(EditDefaultsOnly, Category = "Config", meta = (ClampMin = 1.f, ClampMax = 100.f))
	int32 StartingHealth;

	/** Starting shield value */
	UPROPERTY(EditDefaultsOnly, Category = "Config", meta = (ClampMin = 0.f, ClampMax = 100.f))
	int32 StartingArmour;

	/** Curent health, Initialised in BeginPlay() */
	int32 CurrentHealth;

	/** Current shield, Initialised in BeginPlay() */
	int32 CurrentArmour;

	/** Name of wheel bones to get collision bodies from */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TArray<FName> WheelBodies;

	/** Time till tank is destroyed */
	UPROPERTY(EditDefaultsOnly, Category = "Config", meta = (ClampMin = 1.f, ClampMax = 10.f))
	float DestroyTimer;

	/** Track if tank is alive */
	bool bHasBeenDestroyed;

public:
	ATank();

	/** Bind functionality to input */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Decrease health and play damage fx */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser) override;

	/** Return current health as a percentage of starting health, between 0 and 1 */
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealthPercent() const { return (float)CurrentHealth / (float)StartingHealth; }

	/** Return current health as a percentage of starting health, between 0 and 1 */
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetArmourPercent() const { return (float)CurrentArmour / (float)StartingArmour; }

	/** Replenish health */
	void ReplenishHealth(float HealthToAdd);

	/** Replenish shield */
	void ReplenishArmour();

	/** Return if tank  */
	bool IsTankDestroyed() const { return bHasBeenDestroyed; }

	/** Return if current health is lower than starting health */
	bool IsTankDamaged() const { return CurrentHealth < StartingHealth; }

	/** Death delegate for controllers */
	FTankDelegate OnDeath;

protected:
	virtual void BeginPlay() override;

	/** Fire aiming component */
	void Fire();

	UFUNCTION(BlueprintImplementableEvent, Category = "Health")
	void OnTankDestroyed(bool bDestroyed);

private:
	
	/** Death behaviour */
	void OnDeathBehaviour(AController * EventInstigator);

	/** Notify player controller to update player ui */
	void UpdatePlayerHud();

};
