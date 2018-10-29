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
	
public:
	UTankMovement();

	UFUNCTION(BlueprintCallable, Category = Setup)
	void Initialise(UTrack * LeftTrackToSet, UTrack * RightTrackToSet);

	UFUNCTION(BlueprintCallable, Category = Input)
	void IntendMoveForward(float Throw);

	UFUNCTION(BlueprintCallable, Category = Input)
	void IntendTurnRight(float Throw);

private:
	virtual void RequestDirectMove(const FVector & MoveVelocity, bool bForceMaxSpeed) override;

private:
	UTrack * LeftTrack = nullptr;
	UTrack * RightTrack = nullptr;
	
};
