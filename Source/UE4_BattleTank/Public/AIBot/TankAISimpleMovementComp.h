// Copyright 2018 - 2022 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "SimpleWheeledVehicleMovementComponent.h"
#include "TankAISimpleMovementComp.generated.h"

class ATank;

/**
* Moves Ai Tank using local offsets and rotation
*/
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UE4_BATTLETANK_API UTankAISimpleMovementComp : public USimpleWheeledVehicleMovementComponent
{
	GENERATED_BODY()
	
private:

	/** Owning actor of this component */
	ATank* TankOwner;

	USkeletalMeshComponent * TankMesh;

	////////////////////////////////////////////////////////////////////////////////
	// Drive data

	/** Are wheel brakes currently on */
	bool bBrakesApplied;

	/** Current Forward Value */
	float CurrentForwardValue;

	float WheelTorque;

	/** Rotation force added to each wheel */
	UPROPERTY(EditdefaultsOnly, Category = "Config")
	float ForcePerWheel;

	/** Rotation force added to mesh */
	UPROPERTY(EditdefaultsOnly, Category = "Config")
	float TurnForce;

	/** Rate of max forward speed */
	UPROPERTY(EditdefaultsOnly, Category = "Config", meta = (ClampMin = 0.f))
	float BrakeTorque;


public:
	UTankAISimpleMovementComp();


	////////////////////////////////////////////////////////////////////////////////
	// Drive tank

	/** Add force to tracks for forward movement */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void IntendMoveForward(float value);

	/** Add force to tracks for turning movement */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void IntendTurnRight(float value);

	void SetBrakesValue(bool bSetBrake);

	float GetForwardValue() const { return CurrentForwardValue; }

	float GetForwardTorque() const { return WheelTorque; }

protected:
	virtual void BeginPlay() override;

private:

	////////////////////////////////////////////////////////////////////////////////
	// Drive data

	/** Add torque to wheels to restrict ai movement to same as players */
	virtual void RequestDirectMove(const FVector& MoveVelocity, bool bForceMaxSpeed) override;

	void MovementValuesForAnimation();

	float GetLeftWheelSpeed();

	float GetRightWheelSpeed();

};
