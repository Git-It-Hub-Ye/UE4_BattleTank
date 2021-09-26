// Copyright 2018 - 2021 Stuart McDonald.

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

	////////////////////////////////////////////////////////////////////////////////
	// Drive data

	/** Are wheel brakes currently on */
	bool bBrakesApplied;

	float CurrentForwardSpeed;

	float CurrentTurningSpeed;

	/** Rate of max forward speed */
	UPROPERTY(EditdefaultsOnly, Category = "Config", meta = (ClampMin = 0.f, ClampMax = 1000.f))
	float ForwardMovementRate;


	////////////////////////////////////////////////////////////////////////////////
	// Tank rotation

	/** How fast to rotate tank (Higher values = faster) */
	UPROPERTY(EditdefaultsOnly, Category = "Config", meta = (ClampMin = 0.f, ClampMax = 1.f))
	float TurnRate;


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

	void SetBrakesAtStart();

	float GetForwardValue() const { return CurrentForwardSpeed; }

	float GetTurnRightValue() const { return CurrentTurningSpeed; }

protected:
	virtual void BeginPlay() override;

private:

	////////////////////////////////////////////////////////////////////////////////
	// Drive tank

	/** Add torque to wheels to restrict ai movement to same as players */
	virtual void RequestDirectMove(const FVector& MoveVelocity, bool bForceMaxSpeed) override;

	void MovementValuesForAnimation();
};
