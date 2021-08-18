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

	bool bIsBraking;


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

	/** Max range of Wheel Speed for track animation. Closer to actual speed of wheel will create more accurate movement. */
	UPROPERTY(EditDefaultsOnly, Category = "Config Track", meta = (ClampMin = 1.f, ClampMax = 1000.f))
	float MaxTrackWheelSpeed;

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
	UParticleSystemComponent* ParticleComp;

	/** Destroy fx */
	UPROPERTY(EditDefaultsOnly, Category = "Particles")
	UParticleSystem* DestroyedFX;


	////////////////////////////////////////////////////////////////////////////////
	// SFX

	/** Audio component for this class */
	UPROPERTY()
	UAudioComponent* CollisionAudioComp;

	/** Component for engine sound */
	UPROPERTY(VisibleDefaultsOnly)
	UAudioComponent* EngineAudioComp;

	/** Component for track sound */
	UPROPERTY(VisibleDefaultsOnly)
	UAudioComponent* TrackAudioComp;

	/** Component for stress sound */
	UPROPERTY(VisibleDefaultsOnly)
	UAudioComponent* StressAudioComp;

	/** Sound of tank being destroyed */
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase* DestroyedSound;

	/** Sounds of tank impacting metal objects */
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase* HighImpactSound;

	/** Sounds of tank impacting landscape */
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase* LowImpactSound;

	/** Fastest track for track sfx */
	float SFXTrackSpeedValue;

	/** Track sfx fade out time */
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	float FadeOutTime_Track;

	/** Stress sfx fade out time */
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	float FadeOutTime_Stress;

	bool bIsColliding = false;
	
public:

	ATankVehicle(const FObjectInitializer& ObjectInitializer);


	////////////////////////////////////////////////////////////////////////////////
	// Input

	/** Bind functionality to input */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Stop tank */
	void ApplyBrakes(bool bApplyBrake);


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

	void ComponentCollision(const FHitResult& Impact) { PlayTankCollisionFX(Impact, LowImpactSound); }


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

	/** Gets wheel speeds for track animations and sfx */
	void ApplyInputAnimationValues();

	void SetMovementComp();

	////////////////////////////////////////////////////////////////////////////////
	// Track material animation

	/** Create dynamic material */
	UMaterialInstanceDynamic* SetTrackMats(int32 Element, UMaterialInterface* Mat);

	/** Set material of left track */
	void SetLeftTrackMat(UMaterialInstanceDynamic* Mat);

	/** Set material of right track */
	void SetRightTrackMat(UMaterialInstanceDynamic* Mat);

	/** Set track speed */
	void AnimateTracks(float LeftRangeValue, float RightRangeValue);

	/** Animate left track */
	void AnimateTrackMatLeft(float PositionOffset);

	/** Animate right track */
	void AnimateTrackMatRight(float PositionOffset);


	////////////////////////////////////////////////////////////////////////////////
	// HUD

	/** Notify player controller to update player ui */
	void UpdatePlayerHud();


	////////////////////////////////////////////////////////////////////////////////
	// SFX

	/** Activates on hit */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Play collision sound */
	void PlayTankCollisionFX(const FHitResult& Impact, USoundBase* ImpactSFX);

	/** Set pitch and volume of sound */
	void TankDriveSFX();

	/** Play sound on tank */
	UAudioComponent* SFXPlay(USoundBase* SoundFX);

	/** Stop sound on tank */
	void SFXStop(UAudioComponent* AudioComp);

	/** Stops audio */
	void StopAudioSound();

};
