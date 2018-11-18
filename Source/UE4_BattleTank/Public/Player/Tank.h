// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Tank.generated.h"

class UAimingComponent;
class UCameraShake;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTankDelegate);

UCLASS()
class UE4_BATTLETANK_API ATank : public APawn
{
	GENERATED_BODY()

protected:
	/** Aim tank weapons */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UAimingComponent * AimingComp = nullptr;

private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh")
	USceneComponent * TankRoot;

	/** Root mesh */
	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh")
	USkeletalMeshComponent * TankBody;

	/** Camera shake when damaged */
	UPROPERTY(EditDefaultsOnly, Category = "FX")
	TSubclassOf<UCameraShake> DamageCamShakeBP;

	/** Starting health */
	UPROPERTY(EditDefaultsOnly, Category = Setup)
	int32 StartingHealth = 100;

	/** Curent health */
	int32 CurrentHealth; // Initialised in BeginPlay().

	/** Track if tank is alive */
	bool bHasBeenDestroyed;

public:
	/** Bind functionality to input */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Decrease health and play damage fx */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser) override;

	/** Return current health as a percentage of starting health, between 0 and 1 */
	UFUNCTION(BlueprintPure, Category = Health)
	float GetHealthPercent() const;

	/** Replenish health */
	void ReplenishHealth(float HealthToAdd);

	/** Return if tank  */
	bool IsTankDestroyed() const { return bHasBeenDestroyed; }

	/** Return if current health is lower than starting health */
	bool IsTankDamaged() { return CurrentHealth < StartingHealth; }

	/** Death delegate for controllers */
	FTankDelegate OnDeath;

protected:
	virtual void BeginPlay() override;

	/** Fire aiming component */
	void Fire();

private:
	ATank();
	
};
