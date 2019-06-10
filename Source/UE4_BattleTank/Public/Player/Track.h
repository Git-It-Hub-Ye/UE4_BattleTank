// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Track.generated.h"

class UWheelComponent;

/**
* Track is used to set maximum driving force and apply force to the wheels
*/

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UE4_BATTLETANK_API UTrack : public UStaticMeshComponent
{
	GENERATED_BODY()

private:
	/** Max force to add to track */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	float TrackMaxDrivingForce = 400000;

	/** How fast wheel rotates in animation bp */
	UPROPERTY(EditDefaultsOnly, Category = "Config", meta = (ClampMin = -100.f, ClampMax = 100.f))
	float FrontAndRearWheelSpeed = 1.f;

	/** Current set speed for wheels */
	float CurrentWheelSpeed = 0.f;

	/** Name of wheel bones to get constraints from */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TArray<FName> WheelBoneNames;

	/** Array of wheel bones */
	TArray<FName> WheelBones;

	/** Array of wheel components */
	TArray<UWheelComponent*> WheelComponents;

	/** Array of wheel bodies to apply force too */
	TArray<FConstraintInstance*> WheelConstraints;

	/** Root owner of this track */
	USkeletalMeshComponent * RootBody;

	/** Brakes applied */
	bool bBrake = false;

	/** If turning, used to control wheel speed */
	bool bTurningHalfSpeed = false;

public:
	UTrack();

	/** Set how much force to add to track */
	void SetThrottle(float Throttle, bool bIsTurning);

	/** Restrict wheel movement */
	void ApplyBrakes();

	/** Set front and rear wheel rotation speed for animation bp */
	void SetFrontAndRearWheelSpeed(float Throttle);

	/** Returns current speed of wheels */
	float GetFrontAndRearWheelSpeed() const { return CurrentWheelSpeed; }

protected:
	virtual void BeginPlay() override;

private:
	/** Get all wheel bones and constraints */
	void GetWheels();

	/** Apply force to each wheel bone */
	void DriveWheels(float CurrentThrottle);

	/** Limit constraint movement */
	void LimitWheelBoneConstraints(bool bBraking);

};
