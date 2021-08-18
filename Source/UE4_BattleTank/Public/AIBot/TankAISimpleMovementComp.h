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
	// Drive wheel data

	/** Are wheel brakes currently on */
	bool bBrakesApplied;

	float MoveForwardValue;

	float TurnRightValue;

	/** Rate of max forward speed */
	UPROPERTY(EditdefaultsOnly, Category = "Config", meta = (ClampMin = 0.f, ClampMax = 1000.f))
	float ForwardMovementRate;


	////////////////////////////////////////////////////////////////////////////////
	// Tank rotation

	/** How fast to rotate tank (Higher values = faster) */
	UPROPERTY(EditdefaultsOnly, Category = "Config", meta = (ClampMin = 0.f, ClampMax = 100.f))
	float TurnRate;

	/** Speed to rotate tank */
	float TurnSpeed;

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

	/** Applies brake torque for all wheels */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void ApplyBrakes(bool bApplyBrakes);

	float GetForwardValue() const { return MoveForwardValue; }

	float GetTurnRightValue() const { return TurnRightValue; }

protected:
	virtual void BeginPlay() override;

private:

	////////////////////////////////////////////////////////////////////////////////
	// Drive tank

	/** Add torque to wheels to restrict ai movement to same as players */
	virtual void RequestDirectMove(const FVector& MoveVelocity, bool bForceMaxSpeed) override;

	void MovementValuesForAnimation();
};
