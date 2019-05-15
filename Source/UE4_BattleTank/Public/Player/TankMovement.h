// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/NavMovementComponent.h"
#include "TankMovement.generated.h"

class UTrack;
class USoundBase;
class UAudioComponent;

/**
* Responsible for driving the tank wheels.
*/
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UE4_BATTLETANK_API UTankMovement : public UNavMovementComponent
{
	GENERATED_BODY()
	
private:
	/** Left track reference */
	UTrack * LeftTrack = nullptr;

	/** Right track reference */
	UTrack * RightTrack = nullptr;

	/** Tank engine sound loop */
	UPROPERTY(EditdefaultsOnly, Category = "Audio")
	USoundBase * EngineLoopSfx;

	/** Max pitch for tank sound, when moving fast */
	UPROPERTY(EditAnywhere, Category = "Audio", meta = (ClampMin = 0.f, ClampMax = 2.f))
	float MaxSoundPitch = 2.f;

	/** Max pitch for tank sound, when rotating fast */
	UPROPERTY(EditAnywhere, Category = "Audio", meta = (ClampMin = 0.f, ClampMax = 2.f))
	float MaxTurnSoundPitch = 2.f;

	/** Min pitch for tank sound, when rotating slow */
	UPROPERTY(EditAnywhere, Category = "Audio", meta = (ClampMin = 0.f, ClampMax = 2.f))
	float MinSoundPitch = 1.f;

	/** Component for start & end sounds */
	UAudioComponent * EngineAudio = nullptr;

	/** Is tank turning */
	bool bTurningRight = false;

public:
	UTankMovement();

	/** Initialise the tracks from owner */
	UFUNCTION(BlueprintCallable, Category = "Setup")
	void Initialise(UTrack * LeftTrackToSet, UTrack * RightTrackToSet);

	/** Add force to tracks for forward movement */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void IntendMoveForward(float Throw);

	/** Add force to tracks for turning movement */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void IntendTurnRight(float Throw);

	/** Speed of wheel for this track */
	UFUNCTION(BlueprintCallable, Category = "AnimBP")
	float GetRightTrackWheelSpeed() const;

	/** Speed of wheel for this track */
	UFUNCTION(BlueprintCallable, Category = "AnimBP")
	float GetLeftTrackWheelSpeed() const;

protected :
	/** Stops engine sound */
	void StopEngineSound();

private:
	/** Add force to tracks to restrict ai movement to same as players */
	virtual void RequestDirectMove(const FVector & MoveVelocity, bool bForceMaxSpeed) override;

	UFUNCTION()
	void OnOwnerDeath();

	/** How fast is tank moving */
	float GetMovementSpeed(float Throw);

	/** Set pitch of sound */
	void TankSFXPitch(float PitchRange);

	/** Play sound on tank */
	UAudioComponent * SFXPlay(USoundBase * SoundFX);

};
