// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Track.generated.h"

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

	/** Name of wheel bones to get constraints from */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TArray<FName> WheelBoneNames;

	/** Array of wheel bones */
	TArray<FName> WheelBones;

	/** Array of wheel bodies to apply force too */
	TArray<FConstraintInstance*> WheelConstraints;

	/** Root owner of this track */
	USkeletalMeshComponent * RootBody;

	/** Brakes applied */
	bool bBrake = false;

public:
	UTrack();

	/** Set how much force to add to track */
	void SetThrottle(float Throttle);

	/** Restrict wheel movement */
	void ApplyBrakes();

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
