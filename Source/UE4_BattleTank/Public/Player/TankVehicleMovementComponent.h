// Copyright 2018 - 2021 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "WheeledVehicleMovementComponent.h"
#include "Curves/CurveFloat.h"
#include "TankVehicleMovementComponent.generated.h"

USTRUCT()
struct FTankVehicleEngineData {
	GENERATED_USTRUCT_BODY()

	/** Torque (Nm) at a given RPM*/
	UPROPERTY(EditAnywhere, Category = Setup)
	FRuntimeFloatCurve TorqueCurve;

	/** Maximum revolutions per minute of the engine */
	UPROPERTY(EditAnywhere, Category = Setup, meta = (ClampMin = "0.01", UIMin = "0.01"))
	float MaxRPM;

	/** Moment of inertia of the engine around the axis of rotation (Kgm^2). */
	UPROPERTY(EditAnywhere, Category = Setup, meta = (ClampMin = "0.01", UIMin = "0.01"))
	float MOI;

	/** Damping rate of engine when full throttle is applied (Kgm^2/s) */
	UPROPERTY(EditAnywhere, Category = Setup, AdvancedDisplay, meta = (ClampMin = "0.0", UIMin = "0.0"))
	float DampingRateFullThrottle;

	/** Damping rate of engine in at zero throttle when the clutch is engaged (Kgm^2/s)*/
	UPROPERTY(EditAnywhere, Category = Setup, AdvancedDisplay, meta = (ClampMin = "0.0", UIMin = "0.0"))
	float DampingRateZeroThrottleClutchEngaged;

	/** Damping rate of engine in at zero throttle when the clutch is disengaged (in neutral gear) (Kgm^2/s)*/
	UPROPERTY(EditAnywhere, Category = Setup, AdvancedDisplay, meta = (ClampMin = "0.0", UIMin = "0.0"))
	float DampingRateZeroThrottleClutchDisengaged;

	/** Find the peak torque produced by the TorqueCurve */
	float FindPeakTorque() const;
};

USTRUCT()
struct FTankVehicleGearData {
	GENERATED_USTRUCT_BODY()

	/** Determines the amount of torque multiplication*/
	UPROPERTY(EditAnywhere, Category = Setup)
	float Ratio;

	/** Value of engineRevs/maxEngineRevs that is low enough to gear down*/
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", UIMin = "0.0", ClampMax = "1.0", UIMax = "1.0"), Category = Setup)
	float DownRatio;

	/** Value of engineRevs/maxEngineRevs that is high enough to gear up*/
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", UIMin = "0.0", ClampMax = "1.0", UIMax = "1.0"), Category = Setup)
	float UpRatio;
};

USTRUCT()
struct FTankVehicleTransmissionData {
	GENERATED_USTRUCT_BODY()

	/** Whether to use automatic transmission */
	UPROPERTY(EditAnywhere, Category = VehicleSetup, meta = (DisplayName = "Automatic Transmission"))
	bool bUseGearAutoBox;

	/** Time it takes to switch gears (seconds) */
	UPROPERTY(EditAnywhere, Category = Setup, meta = (ClampMin = "0.0", UIMin = "0.0"))
	float GearSwitchTime;

	/** Minimum time it takes the automatic transmission to initiate a gear change (seconds)*/
	UPROPERTY(EditAnywhere, Category = Setup, meta = (editcondition = "bUseGearAutoBox", ClampMin = "0.0", UIMin = "0.0"))
	float GearAutoBoxLatency;

	/** The final gear ratio multiplies the transmission gear ratios.*/
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = Setup)
	float FinalRatio;

	/** Forward gear ratios (up to 30) */
	UPROPERTY(EditAnywhere, Category = Setup, AdvancedDisplay)
	TArray<FTankVehicleGearData> ForwardGears;

	/** Reverse gear ratio */
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = Setup)
	float ReverseGearRatio;

	/** Value of engineRevs/maxEngineRevs that is high enough to increment gear*/
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = Setup, meta = (ClampMin = "0.0", UIMin = "0.0", ClampMax = "1.0", UIMax = "1.0"))
	float NeutralGearUpRatio;

	/** Strength of clutch (Kgm^2/s)*/
	UPROPERTY(EditAnywhere, Category = Setup, AdvancedDisplay, meta = (ClampMin = "0.0", UIMin = "0.0"))
	float ClutchStrength;
};

PRAGMA_DISABLE_DEPRECATION_WARNINGS

/**
 * Drive and Rotate Tank using Physx (This is a customised class similar to WheeledVehicleMovementComponent4W making use of PxVehicleDriveTank within Physx)
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UE4_BATTLETANK_API UTankVehicleMovementComponent : public UWheeledVehicleMovementComponent
{
	GENERATED_BODY()

public:

	/** Engine */
	UPROPERTY(EditAnywhere, Category = "MechanicalSetup")
	FTankVehicleEngineData EngineSetup;

	/** Transmission data */
	UPROPERTY(EditAnywhere, Category = "MechanicalSetup")
	FTankVehicleTransmissionData TransmissionSetup;

protected:

	/** Current Steer value for left track */
	float ThrustInput_LeftTrack;

	/** Current Steer value for right track */
	float ThrustInput_RightTrack;
	
public:

	UTankVehicleMovementComponent(const FObjectInitializer& ObjectInitializer);

	virtual void Serialize(FArchive& Ar) override;

	virtual void ComputeConstants() override;

#if WITH_EDITOR

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

#endif

protected:

	/** Read current state for simulation */
	virtual void UpdateState(float DeltaTime) override;

#if WITH_PHYSX && PHYSICS_INTERFACE_PHYSX

	/** Allocate and setup the PhysX vehicle */
	virtual void SetupVehicle() override;

	/** Pass input values to vehicle simulation */
	virtual void UpdateSimulation(float DeltaTime) override;

#endif // WITH_PHYSX
	
	/** Update simulation data: engine */
	void UpdateEngineSetup(const FTankVehicleEngineData& NewEngineSetup);

	/** Update simulation data: transmission */
	void UpdateTransmissionSetup(const FTankVehicleTransmissionData& NewGearSetup);

private:

	/** Calculates tank braking (customised for turning in place) */
	float CalcTankBrakeInput();

	/** Calculates Left track steer value */
	float CalcTrackSteer_Left();

	/** Calculates Right track steer value */
	float CalcTrackSteer_Right();
	
};

PRAGMA_ENABLE_DEPRECATION_WARNINGS
