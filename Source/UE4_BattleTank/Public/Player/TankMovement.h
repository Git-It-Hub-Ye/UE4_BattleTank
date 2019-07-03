// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "SimpleWheeledVehicleMovementComponent.h"
#include "TankMovement.generated.h"

class ATank;

/**
* Drives wheels and rotates tank
*/
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UE4_BATTLETANK_API UTankMovement : public USimpleWheeledVehicleMovementComponent
{
	GENERATED_BODY()

private:

	/** Owning actor of this component */
	ATank * TankOwner;

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

	/** Are wheel brakes currently on */
	bool bBrakesApplied;

	float MoveForwardValue;

	float TurnRightValue;


	////////////////////////////////////////////////////////////////////////////////
	// Tank rotation

	/** How fast to rotate tank (Higher values = faster) */
	UPROPERTY(EditdefaultsOnly, Category = "Config", meta = (ClampMin = 0.f, ClampMax = 1.f))
	float TurnRate;

	/** Speed to rotate tank */
	float TurnSpeed;

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

	/** Applies brake torque for all wheels */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void ApplyBrakes(bool bApplyBrakes);

	float GetForwardValue() { return MoveForwardValue; }

	float GetTurnRightValue() { return TurnRightValue; }

protected:
	virtual void BeginPlay() override;

private:

	////////////////////////////////////////////////////////////////////////////////
	// Drive tank

	/** Add torque to wheels to restrict ai movement to same as players */
	virtual void RequestDirectMove(const FVector & MoveVelocity, bool bForceMaxSpeed) override;

	/** Sets drive torque for each wheel on right side */
	void DriveRightWheels(float Throttle);

	/** Sets drive torque for each wheel on left side */
	void DriveLeftWheels(float Throttle);

	void MovementValuesForAnimation();

};
