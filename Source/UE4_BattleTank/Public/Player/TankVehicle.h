// Copyright 2018 - 2021 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehicle.h"
#include "TankVehicle.generated.h"

class UAimingComponent;
class UTankVehicleMovementComponent;
class UAudioComponent;
class UParticleSystemComponent;

/**
 * Tank vehicle, can be controlled by player and ai
 */
UCLASS()
class UE4_BATTLETANK_API ATankVehicle : public AWheeledVehicle {
	GENERATED_BODY()

protected:
	////////////////////////////////////////////////////////////////////////////////
	// Components

	/** Aim tank weapons */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UAimingComponent* AimingComp = nullptr;

	/** Audio component for this class */
	UPROPERTY()
	UAudioComponent* AudioComp;


	////////////////////////////////////////////////////////////////////////////////
	// Animation wheel data

	/** Speed of turning for right wheels */
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float RightWheelYaw;

	/** Speed of turning for left wheels */
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float LeftWheelYaw;

	/** Speed of turning for right front and back wheels */
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float RightFrontBackYaw;

	/** Speed of turning for left front and back wheels */
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float LeftFrontBackYaw;

private:

	////////////////////////////////////////////////////////////////////////////////////
	// Tank data

	/** Movement tank comp */
	UTankVehicleMovementComponent* MovementComp = nullptr;

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

	/** Time till tank is destroyed */
	UPROPERTY(EditDefaultsOnly, Category = "Config", meta = (ClampMin = 1.f, ClampMax = 10.f))
	float DestroyTimer;

	/** Track if tank is alive */
	bool bHasBeenDestroyed;

	/** Previous saved rotation of tank */
	FRotator LastYawRot;


	////////////////////////////////////////////////////////////////////////////////
	// Animation

	/** How much to multiply wheel and track rotation by (Higher values = faster) */
	UPROPERTY(EditdefaultsOnly, Category = "Wheel/Track", meta = (ClampMin = 0.f))
	float TurnSpeedMultiplier;

	/** Crosshair to display on player ui for this weapon */
	UPROPERTY(EditDefaultsOnly, Category = "Wheel/Track")
	UMaterialInterface* TrackMat;

	/** Element of tank to apply dynamic track material */
	UPROPERTY(EditdefaultsOnly, Category = "Wheel/Track", meta = (ClampMin = 0.f))
	int32 LeftTrackElement;

	/** Element of tank to apply dynamic track material */
	UPROPERTY(EditdefaultsOnly, Category = "Wheel/Track", meta = (ClampMin = 0.f))
	int32 RightTrackElement;

	/** Name of track material parameter to animate */
	UPROPERTY(EditdefaultsOnly, Category = "Wheel/Track")
	FName TrackScalarParamName;

	/** Dynamic material for track rotation */
	UMaterialInstanceDynamic* LeftTrackMat;

	/** Dynamic material for track rotation */
	UMaterialInstanceDynamic* RightTrackMat;

	/** Speed of left track material animation */
	float LeftTrackSpeed;

	/** Speed of right track material animation */
	float RightTrackSpeed;


	////////////////////////////////////////////////////////////////////////////////////
	// VFX

	/** Component for fx on actor */
	UParticleSystemComponent* ParticleComp;

	/** Destroy fx */
	UPROPERTY(EditDefaultsOnly, Category = "Particles")
	UParticleSystem* DestroyedFX;


	////////////////////////////////////////////////////////////////////////////////
	// SFX

	/** Component for start & end sounds */
	UAudioComponent* EngineAudio = nullptr;

	/** Tank engine sound loop */
	UPROPERTY(EditdefaultsOnly, Category = "Audio")
	USoundBase* EngineLoopSfx;

	/** Max pitch for tank sound, when moving fast */
	UPROPERTY(EditAnywhere, Category = "Audio", meta = (ClampMin = 0.f, ClampMax = 2.f))
	float MaxSoundPitch = 2.f;

	/** Min pitch for tank sound, when rotating slow */
	UPROPERTY(EditAnywhere, Category = "Audio", meta = (ClampMin = 0.f, ClampMax = 2.f))
	float MinSoundPitch = 1.f;

	/** Max pitch for tank sound, when rotating fast */
	UPROPERTY(EditAnywhere, Category = "Audio", meta = (ClampMin = 0.f, ClampMax = 2.f))
	float MaxTurnSoundPitch = 2.f;

	/** Sound of tank being destroyed */
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase* DestroyedSound;

	/** Sounds of tank impacting metal objects */
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase* MetalImpactSound;

	/** Sounds of tank impacting landscape */
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase* LandImpactSound;

public:
	ATankVehicle(const FObjectInitializer& ObjectInitializer);


	////////////////////////////////////////////////////////////////////////////////
	// Input

	/** Bind functionality to input */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Stop tank */
	void ApplyBrakes(bool bApplyBrake);

	/** Gets actor speed for wheel, track animations and sfx */
	void ApplyInputMovementBehaviours(float TurnRate, float TurnSpeed);


	////////////////////////////////////////////////////////////////////////////////
	// Tank Behaviour

	/** Replenish health */
	void ReplenishHealth(float HealthToAdd);

	/** Replenish shield */
	void ReplenishArmour();

	void Execute();

	/** Death delegate for controllers */
	//FTankDelegate OnDeath;


	////////////////////////////////////////////////////////////////////////////////
	// Tank Data

	/** Return current health as a percentage of starting health, between 0 and 1 */
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealthPercent() const { return (float)CurrentHealth / (float)StartingHealth; }

	/** Return current health as a percentage of starting health, between 0 and 1 */
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetArmourPercent() const { return (float)CurrentArmour / (float)StartingArmour; }

	/** Return if tank  */
	bool IsTankDestroyed() const { return bHasBeenDestroyed; }

	/** Return if current health is lower than starting health */
	bool IsTankDamaged() const { return CurrentHealth < StartingHealth; }


	////////////////////////////////////////////////////////////////////////////////
	// HUD

	/** Notifies owner they are outside play area */
	void OutOfCombatArea(bool bWarnPlayer);

protected:
	virtual void BeginPlay() override;


	////////////////////////////////////////////////////////////////////////////////
	// Input

	/** Fire aiming component */
	void Fire();

	/** Move tank forward or back with gamepad */
	void MoveForward(float Value);

	/** Move tank right and left with gamepad */
	void TurnRight(float Value);

private:

	////////////////////////////////////////////////////////////////////////////////
	// Tank behaviour

	/** Decrease health and play damage fx */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	/** Death behaviour */
	void OnDeathBehaviour(AController* EventInstigator);

	void SetMovementComp();


	////////////////////////////////////////////////////////////////////////////////
	// Wheel animation

	/** Set wheel rotation of tank */
	void TurnWheels(float ForwardSpeed, float TurnSpeed);

	/** Set wheel rotation value to apply */
	float SetWheelTurnValue(float TurnSpeed);


	////////////////////////////////////////////////////////////////////////////////
	// Track material animation

	/** Create dynamic material */
	UMaterialInstanceDynamic* SetTrackMats(int32 Element, UMaterialInterface* Mat);

	/** Set material of left track */
	void SetLeftTrackMat(UMaterialInstanceDynamic* Mat);

	/** Set material of right track */
	void SetRightTrackMat(UMaterialInstanceDynamic* Mat);

	/** Set track speed */
	void AnimateTracks(float ForwardSpeed, float TurnSpeed);

	/** Animate left track */
	void AnimateTrackMatLeft(float NewOffset);

	/** Animate right track */
	void AnimateTrackMatRight(float NewOffset);


	////////////////////////////////////////////////////////////////////////////////
	// HUD

	/** Notify player controller to update player ui */
	void UpdatePlayerHud();


	////////////////////////////////////////////////////////////////////////////////
	// SFX

	/** Sends hit result to find hit objects surface */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Play collision sound */
	void PlayTankCollisionFX(const FHitResult& Impact);

	/** Get sound to play for surface collision */
	USoundBase* GetImpactSound(TEnumAsByte<EPhysicalSurface> SurfaceType) const;

	/** Set pitch of sound */
	void TankSFXPitch(float PitchRange);

	/** Play sound on tank */
	UAudioComponent* SFXPlay(USoundBase* SoundFX);

	/** Stops engine sound */
	void StopEngineSound();

};
