// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Tank.generated.h"

class UAimingComponent;
class UTankMovement;
class UAudioComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTankDelegate);

UCLASS()
class UE4_BATTLETANK_API ATank : public APawn
{
	GENERATED_BODY()

protected:
	////////////////////////////////////////////////////////////////////////////////
	// Components

	/** Root mesh */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent * TankBody;

	/** Aim tank weapons */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UAimingComponent * AimingComp = nullptr;

	/** Move tank */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UTankMovement * MovementComp = nullptr;

	/** Audio component for this class */
	UPROPERTY()
	UAudioComponent * AudioComp;


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


	////////////////////////////////////////////////////////////////////////////////////
	// Input data

	/** Current forward input value */
	float MoveForwardValue;

	/** Current turn input value */
	float TurnRightValue;

	/** Speed to rotate tank */
	float TurnSpeed;

	/** Previous saved rotation of tank */
	FRotator LastYawRot;


	////////////////////////////////////////////////////////////////////////////////
	// Wheel animation

	/** How much to multiply wheel rotation by (Higher values = faster) */
	UPROPERTY(EditdefaultsOnly, Category = "Animate Wheel", meta = (ClampMin = 0.f))
	float WheelTurnMultiplier;

	/** How fast to rotate tank (Higher values = faster) */
	UPROPERTY(EditdefaultsOnly, Category = "Animate Wheel", meta = (ClampMin = 0.f, ClampMax = 1.f))
	float TurnRate;


	////////////////////////////////////////////////////////////////////////////////
	// Track material animation

	/** Crosshair to display on player ui for this weapon */
	UPROPERTY(EditDefaultsOnly, Category = "Animate Track")
	UMaterialInterface * TrackMat;

	/** Element of tank to apply dynamic track material */
	UPROPERTY(EditdefaultsOnly, Category = "Animate Track", meta = (ClampMin = 0.f))
	int32 LeftTrackElement;

	/** Element of tank to apply dynamic track material */
	UPROPERTY(EditdefaultsOnly, Category = "Animate Track", meta = (ClampMin = 0.f))
	int32 RightTrackElement;

	/** Name of track mat parameter to animate */
	UPROPERTY(EditdefaultsOnly, Category = "Animate Track")
	FName TrackScalarParamName;

	/** How much to multiply track animation by (Higher values = faster) */
	UPROPERTY(EditdefaultsOnly, Category = "Animate Track", meta = (ClampMin = 0.f))
	float TrackSpeedMultiplier;

	/** Dynamic material for track rotation */
	UMaterialInstanceDynamic * LeftTrackMat;

	/** Dynamic material for track rotation */
	UMaterialInstanceDynamic * RightTrackMat;

	/** Speed of left track material animation */
	float LeftTrackSpeed;

	/** Speed of right track material animation */
	float RightTrackSpeed;


	////////////////////////////////////////////////////////////////////////////////////
	// VFX

	/** Component for fx on actor */
	UParticleSystemComponent * ParticleComp;

	/** Destroy fx */
	UPROPERTY(EditDefaultsOnly, Category = "Particles")
	UParticleSystem * DestroyedFX;


	////////////////////////////////////////////////////////////////////////////////
	// SFX

	/** Component for start & end sounds */
	UAudioComponent * EngineAudio = nullptr;

	/** Tank engine sound loop */
	UPROPERTY(EditdefaultsOnly, Category = "Audio")
	USoundBase * EngineLoopSfx;

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
	USoundBase * DestroyedSound;

	/** Sounds of tank impacting metal objects */
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase * MetalImpactSound;

	/** Sounds of tank impacting landscape */
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase * LandImpactSound;

public:
	ATank();


	////////////////////////////////////////////////////////////////////////////////
	// Input

	/** Bind functionality to input */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	////////////////////////////////////////////////////////////////////////////////
	// Tank Behaviour

	/** Replenish health */
	void ReplenishHealth(float HealthToAdd);

	/** Replenish shield */
	void ReplenishArmour();

	void Execute();

	/** Death delegate for controllers */
	FTankDelegate OnDeath;


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

	/** Move tank forward or back */
	void MoveForward(float Value);

	/** Move tank right and left */
	void TurnRight(float Value);

private:
	
	////////////////////////////////////////////////////////////////////////////////
	// Tank behaviour

	/** Decrease health and play damage fx */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser) override;

	/** Death behaviour */
	void OnDeathBehaviour(AController * EventInstigator);


	////////////////////////////////////////////////////////////////////////////////
	// Input data

	/** How fast is tank moving */
	void ApplyInputMovementBehaviours();


	////////////////////////////////////////////////////////////////////////////////
	// Wheel animation

	/** Set wheel rotation of tank */
	void TurnWheels(float ForwardSpeed, float TurnSpeed);

	/** Set wheel rotation value to apply */
	float SetWheelTurnValue(float TurnSpeed);


	////////////////////////////////////////////////////////////////////////////////
	// Track material animation

	/** Create dynamic material */
	UMaterialInstanceDynamic * SetTrackMats(int32 Element, UMaterialInterface * Mat);

	/** Set material of left track */
	void SetLeftTrackMat(UMaterialInstanceDynamic * TrackMat);

	/** Set material of right track */
	void SetRightTrackMat(UMaterialInstanceDynamic * TrackMat);

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
	void OnHit(UPrimitiveComponent * HitComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit);

	/** Play collision sound */
	void PlayTankCollisionFX(const FHitResult & Impact);

	/** Get sound to play for surface collision */
	USoundBase * GetImpactSound(TEnumAsByte<EPhysicalSurface> SurfaceType) const;

	/** Set pitch of sound */
	void TankSFXPitch(float PitchRange);

	/** Play sound on tank */
	UAudioComponent * SFXPlay(USoundBase * SoundFX);

	/** Stops engine sound */
	void StopEngineSound();

};
