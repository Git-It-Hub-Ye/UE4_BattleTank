// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "SimpleWheeledVehicleMovementComponent.h"
#include "TankMovement.generated.h"

class UTrack;
class USoundBase;
class UAudioComponent;

/**
* Drives wheels and rotates tank
*/
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UE4_BATTLETANK_API UTankMovement : public USimpleWheeledVehicleMovementComponent
{
	GENERATED_BODY()

protected:

	////////////////////////////////////////////////////////////////////////////////
	// Animation wheel data

	/** Speed of turning */
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float RightWheelYaw;

	/** Speed of turning */
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float LeftWheelYaw;

	/** Speed of turning */
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float LeftFrontBackYaw;

	/** Speed of turning */
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float RightFrontBackYaw;

private:

	////////////////////////////////////////////////////////////////////////////////
	// Drive wheel data

	/** First index for wheel on right side */
	UPROPERTY(EditdefaultsOnly, Category = "Config", meta = (ClampMin = 0))
	int32 FirstRightWheelIndex;

	/** Last index for wheel on right side */
	UPROPERTY(EditdefaultsOnly, Category = "Config", meta = (ClampMin = 0))
	int32 LastRightWheelIndex;

	/** First index for wheel on Left side */
	UPROPERTY(EditdefaultsOnly, Category = "Config", meta = (ClampMin = 0))
	int32 FirstLeftWheelIndex;

	/** Last index for wheel on Left side */
	UPROPERTY(EditdefaultsOnly, Category = "Config", meta = (ClampMin = 0))
	int32 LastLeftWheelIndex;

	/** How much torque to set for driving each wheel */
	UPROPERTY(EditdefaultsOnly, Category = "Config", meta = (ClampMin = 0))
	float DriveTorquePerWheel;

	/** How much torque to set for braking each wheel */
	UPROPERTY(EditdefaultsOnly, Category = "Config", meta = (ClampMin = 0))
	float BrakeTorquePerWheel;

	/** Current forward input value */
	float MoveForwardValue;

	/** Current turn input value */
	float TurnRightValue;

	/** Are wheel brakes currently on */
	bool bBrakesApplied;


	////////////////////////////////////////////////////////////////////////////////
	// Tank rotation

	/** How fast to rotate tank (Higher values = faster) */
	UPROPERTY(EditdefaultsOnly, Category = "Config", meta = (ClampMin = 0.f, ClampMax = 1.f))
	float TurnRate;

	/** How much to multiply wheel rotation by (Higher values = faster) */
	UPROPERTY(EditdefaultsOnly, Category = "Config", meta = (ClampMin = 0.f))
	float WheelTurnMultiplier;

	/** Speed to rotate tank */
	float TurnSpeed;

	/** Previous saved rotation of tank */
	FRotator LastYawRot;


	////////////////////////////////////////////////////////////////////////////////
	// Track material animation

	/** Dynamic material for track rotation */
	UMaterialInstanceDynamic * LeftTrackMat;

	/** Dynamic material for track rotation */
	UMaterialInstanceDynamic * RightTrackMat;

	/** Name of track mat parameter to animate */
	UPROPERTY(EditdefaultsOnly, Category = "Config")
	FName TrackScalarParamName;

	/** How much to multiply track animation by (Higher values = faster) */
	UPROPERTY(EditdefaultsOnly, Category = "Config", meta = (ClampMin = 0.f))
	float TrackSpeedMultiplier;

	/** Speed of left track material animation */
	float LeftTrackSpeed;

	/** Speed of right track material animation */
	float RightTrackSpeed;


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

public:
	UTankMovement();


	////////////////////////////////////////////////////////////////////////////////
	// Drive tank

	/** Add force to tracks for forward movement */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void IntendMoveForward(float value);

	/** Add force to tracks for turning movement */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void IntendTurnRight(float value);


	////////////////////////////////////////////////////////////////////////////////
	// Track material

	/** Set material of left track */
	void SetLeftTrackMat(UMaterialInstanceDynamic * TrackMat);

	/** Set material of right track */
	void SetRightTrackMat(UMaterialInstanceDynamic * TrackMat);

protected :
	virtual void BeginPlay() override;

private:

	////////////////////////////////////////////////////////////////////////////////
	// Drive tank

	/** Add force to tracks to restrict ai movement to same as players */
	virtual void RequestDirectMove(const FVector & MoveVelocity, bool bForceMaxSpeed) override;

	/** Sets drive torque for each wheel on right side */
	void DriveRightWheels(float Throttle);

	/** Sets drive torque for each wheel on left side */
	void DriveLeftWheels(float Throttle);

	/** Applies brake torque for all wheels */
	void ApplyBrakes(bool bApplyBrakes);

	/** How fast is tank moving */
	void ApplyMovementSpeedBehaviours();


	////////////////////////////////////////////////////////////////////////////////
	// Animation data

	/** Set wheel rotation of tank */
	void TurnWheels(float ForwardSpeed, float TurnSpeed);

	/** Set wheel rotation value to apply */
	float SetWheelTurnValue(float TurnSpeed);

	/** Set track speed */
	void AnimateTracks(float ForwardSpeed, float TurnSpeed);

	/** Animate left track */
	void AnimateTrackMatLeft(float NewOffset);

	/** Animate right track */
	void AnimateTrackMatRight(float NewOffset);


	////////////////////////////////////////////////////////////////////////////////
	// SFX

	/** Set pitch of sound */
	void TankSFXPitch(float PitchRange);

	/** Play sound on tank */
	UAudioComponent * SFXPlay(USoundBase * SoundFX);

	/** Stops engine sound */
	void StopEngineSound();

	/** Executes behaviour on tank owner death */
	UFUNCTION()
	void OnOwnerDeath();

};
