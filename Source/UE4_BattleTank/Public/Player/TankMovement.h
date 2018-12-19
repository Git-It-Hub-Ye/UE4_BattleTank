// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/NavMovementComponent.h"
#include "TankMovement.generated.h"

class UTrack;

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

private:
	/** Add force to tracks to restrict ai movement to same as players */
	virtual void RequestDirectMove(const FVector & MoveVelocity, bool bForceMaxSpeed) override;

};
