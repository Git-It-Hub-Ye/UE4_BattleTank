// Copyright 2018 - 2021 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Tank.generated.h"

class UAimingComponent;
class UTankAISimpleMovementComp;
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
	UTankAISimpleMovementComp* MovementComp = nullptr;


	////////////////////////////////////////////////////////////////////////////////
	// Animation wheel data

	/** Tank Forward MAX Wheel Speed */
	UPROPERTY(EditDefaultsOnly, Category = "Wheel/Track", meta = (ClampMin = 0.f, ClampMax = 1000.f))
	float MaxForwardWheelSpeed;

	/** Tank Forward Wheel speed Range between MaxForwardWheelSpeed & -MaxForwardWheelSpeed. Set close to actual forard speed */
	UPROPERTY(EditDefaultsOnly, Category = "Wheel/Track", meta = (ClampMin = 0.f, ClampMax = 1000.f))
	float ForwardWheelSpeed_Range;

	/** Tank Turning MAX Wheel Speed */
	UPROPERTY(EditDefaultsOnly, Category = "Wheel/Track", meta = (ClampMin = 0.f, ClampMax = 1000.f))
	float MaxTurningWheelSpeed;

	/** Tank Turning Wheel speed Range between MaxTurningWheelSpeed & -MaxTurningWheelSpeed. Set close to actual turning speed */
	UPROPERTY(EditDefaultsOnly, Category = "Wheel/Track", meta = (ClampMin = 0.f, ClampMax = 1000.f))
	float TurningWheelSpeed_Range;

	/** Speed of turning for right wheels */
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float RightWheelYaw;

	/** Speed of turning for left wheels */
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float LeftWheelYaw;


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

	/** Is tank braking */
	bool bIsBraking;

	/** Current movement speed */
	float CurrentSpeed;


	////////////////////////////////////////////////////////////////////////////////
	// Animation

	/** Element of mesh to apply dynamic track material */
	UPROPERTY(EditdefaultsOnly, Category = "Config Track", meta = (ClampMin = 0.f))
	int32 LeftTrackElement;

	/** Element of mesh to apply dynamic track material */
	UPROPERTY(EditdefaultsOnly, Category = "Config Track", meta = (ClampMin = 0.f))
	int32 RightTrackElement;

	/** Name of track material parameter to animate */
	UPROPERTY(EditdefaultsOnly, Category = "Config Track")
	FName TrackScalarParamName;

	/** Range between MaxTrackWheelSpeed & -MaxTrackWheelSpeed, adjusting this value will alter track speed easier. A value of 0 will stop track movement, higher value will speed up. */
	UPROPERTY(EditDefaultsOnly, Category = "Config Track", meta = (ClampMin = 0.f, ClampMax = 100.f))
	float TrackSpeed_Range;

	/** Dynamic material for left track rotation */
	UMaterialInstanceDynamic* LeftTrackMat;

	/** Dynamic material for right track rotation */
	UMaterialInstanceDynamic* RightTrackMat;

	/** Current range of left wheel speed */
	float LeftWheelSpeedValue;

	/** Current range of right wheel speed */
	float RightWheelSpeedValue;

	/** UV offset for left track animation */
	float LeftTrackUVOffset;

	/** UV offset for right track animation */
	float RightTrackUVOffset;


	////////////////////////////////////////////////////////////////////////////////////
	// VFX

	/** Component for fx on actor */
	UParticleSystemComponent * ParticleComp;

	/** Destroy fx */
	UPROPERTY(EditDefaultsOnly, Category = "Particles")
	UParticleSystem * DestroyedFX;


	////////////////////////////////////////////////////////////////////////////////
	// SFX

	/** Component for engine sound */
	UPROPERTY(VisibleDefaultsOnly)
	UAudioComponent* EngineAudioComp;

	/** Component for track sound */
	UPROPERTY(VisibleDefaultsOnly)
	UAudioComponent* TrackAudioComp;

	/** Component for stress sound */
	UPROPERTY(VisibleDefaultsOnly)
	UAudioComponent* StressAudioComp;

	/** Component for Collision sound */
	UPROPERTY(VisibleDefaultsOnly)
	UAudioComponent* CollisionAudioComp;

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

	/** Gets actor speed for wheel, track animations and sfx */
	void ApplyInputAnimationValues(float ForwardRate, float TurnRate);

	/** Sets brake */
	void ApplyBrakes(bool bApplyBrakes);


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
	// Wheel animation

	/** Set wheel rotation of tank */
	void TurnWheels(float MaxForwardRotSpeed, float MaxTurningRotSpeed);

	
	////////////////////////////////////////////////////////////////////////////////
	// Track material animation

	/** Create dynamic material */
	UMaterialInstanceDynamic * SetTrackMats(int32 Element, UMaterialInterface * Mat);

	/** Set material of left track */
	void SetLeftTrackMat(UMaterialInstanceDynamic * Track_Mat);

	/** Set material of right track */
	void SetRightTrackMat(UMaterialInstanceDynamic * Trac_kMat);

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

	/** Activates on hit */
	void PlayTankCollisionFX(const FHitResult& Impact, USoundBase* ImpactSFX);

	/** Set pitch and volume of sound */
	void TankDriveSFX();

	/** Stops audio */
	void StopAudioSound();

};
