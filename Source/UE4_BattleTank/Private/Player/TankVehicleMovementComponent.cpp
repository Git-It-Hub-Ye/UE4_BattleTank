// Copyright 2018 - 2021 Stuart McDonald.

#include "TankVehicleMovementComponent.h"
#include "UE4_BattleTank.h"
#include "Components/PrimitiveComponent.h"
#include "PhysicsEngine/BodyInstance.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

UTankVehicleMovementComponent::UTankVehicleMovementComponent(const FObjectInitializer& ObjectInitializer)
{
#if PHYSICS_INTERFACE_PHYSX

	// Grab default values from physx
	PxVehicleEngineData DefEngineData;
	EngineSetup.MOI = DefEngineData.mMOI;
	EngineSetup.MaxRPM = OmegaToRPM(DefEngineData.mMaxOmega);
	EngineSetup.DampingRateFullThrottle = DefEngineData.mDampingRateFullThrottle;
	EngineSetup.DampingRateZeroThrottleClutchEngaged = DefEngineData.mDampingRateZeroThrottleClutchEngaged;
	EngineSetup.DampingRateZeroThrottleClutchDisengaged = DefEngineData.mDampingRateZeroThrottleClutchDisengaged;

	// Convert from PhysX curve to Unreal's
	FRichCurve* TorqueCurveData = EngineSetup.TorqueCurve.GetRichCurve();
	for (PxU32 KeyIdx = 0; KeyIdx < DefEngineData.mTorqueCurve.getNbDataPairs(); KeyIdx++)
	{
		float Input = DefEngineData.mTorqueCurve.getX(KeyIdx) * EngineSetup.MaxRPM;
		float Output = DefEngineData.mTorqueCurve.getY(KeyIdx) * DefEngineData.mPeakTorque;
		TorqueCurveData->AddKey(Input, Output);
	}

	PxVehicleClutchData DefClutchData;
	TransmissionSetup.ClutchStrength = DefClutchData.mStrength;

	PxVehicleGearsData DefGearSetup;
	TransmissionSetup.GearSwitchTime = DefGearSetup.mSwitchTime;
	TransmissionSetup.ReverseGearRatio = DefGearSetup.mRatios[PxVehicleGearsData::eREVERSE];
	TransmissionSetup.FinalRatio = DefGearSetup.mFinalRatio;

	PxVehicleAutoBoxData DefAutoBoxSetup;
	TransmissionSetup.NeutralGearUpRatio = DefAutoBoxSetup.mUpRatios[PxVehicleGearsData::eNEUTRAL];
	TransmissionSetup.GearAutoBoxLatency = DefAutoBoxSetup.getLatency();
	TransmissionSetup.bUseGearAutoBox = true;

	for (uint32 i = PxVehicleGearsData::eFIRST; i < DefGearSetup.mNbRatios; i++)
	{
		FTankVehicleGearData GearData;
		GearData.DownRatio = DefAutoBoxSetup.mDownRatios[i];
		GearData.UpRatio = DefAutoBoxSetup.mUpRatios[i];
		GearData.Ratio = DefGearSetup.mRatios[i];
		TransmissionSetup.ForwardGears.Add(GearData);
	}

	// Initialize WheelSetups array with 4 wheels
	WheelSetups.SetNum(4);

#endif // WITH_PHYSX
}

#if WITH_EDITOR

void UTankVehicleMovementComponent::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	const FName PropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (PropertyName == TEXT("DownRatio"))
	{
		for (int32 GearIdx = 0; GearIdx < TransmissionSetup.ForwardGears.Num(); ++GearIdx)
		{
			FTankVehicleGearData& GearData = TransmissionSetup.ForwardGears[GearIdx];
			GearData.DownRatio = FMath::Min(GearData.DownRatio, GearData.UpRatio);
		}
	}
	else if (PropertyName == TEXT("UpRatio"))
	{
		for (int32 GearIdx = 0; GearIdx < TransmissionSetup.ForwardGears.Num(); ++GearIdx)
		{
			FTankVehicleGearData& GearData = TransmissionSetup.ForwardGears[GearIdx];
			GearData.UpRatio = FMath::Max(GearData.DownRatio, GearData.UpRatio);
		}
	}
}

#endif

// Copy UWheeledVehicleMovementComponent4W FindPeakTorque()
float FTankVehicleEngineData::FindPeakTorque() const
{
	// Find max torque
	float PeakTorque = 0.f;
	TArray<FRichCurveKey> TorqueKeys = TorqueCurve.GetRichCurveConst()->GetCopyOfKeys();
	for (int32 KeyIdx = 0; KeyIdx < TorqueKeys.Num(); KeyIdx++)
	{
		FRichCurveKey& Key = TorqueKeys[KeyIdx];
		PeakTorque = FMath::Max(PeakTorque, Key.Value);
	}
	return PeakTorque;
}

#if PHYSICS_INTERFACE_PHYSX

// Copy UWheeledVehicleMovementComponent4W GetVehicleGearSetup()
static void GetVehicleEngineSetup(const FTankVehicleEngineData& Setup, PxVehicleEngineData& PxSetup)
{
	PxSetup.mMOI = M2ToCm2(Setup.MOI);
	PxSetup.mMaxOmega = RPMToOmega(Setup.MaxRPM);
	PxSetup.mDampingRateFullThrottle = M2ToCm2(Setup.DampingRateFullThrottle);
	PxSetup.mDampingRateZeroThrottleClutchEngaged = M2ToCm2(Setup.DampingRateZeroThrottleClutchEngaged);
	PxSetup.mDampingRateZeroThrottleClutchDisengaged = M2ToCm2(Setup.DampingRateZeroThrottleClutchDisengaged);

	float PeakTorque = Setup.FindPeakTorque(); // In Nm
	PxSetup.mPeakTorque = M2ToCm2(PeakTorque);	// convert Nm to (kg cm^2/s^2)

	// Convert from our curve to PhysX
	PxSetup.mTorqueCurve.clear();
	TArray<FRichCurveKey> TorqueKeys = Setup.TorqueCurve.GetRichCurveConst()->GetCopyOfKeys();
	int32 NumTorqueCurveKeys = FMath::Min<int32>(TorqueKeys.Num(), PxVehicleEngineData::eMAX_NB_ENGINE_TORQUE_CURVE_ENTRIES);
	for (int32 KeyIdx = 0; KeyIdx < NumTorqueCurveKeys; KeyIdx++)
	{
		FRichCurveKey& Key = TorqueKeys[KeyIdx];
		const float KeyFloat = FMath::IsNearlyZero(Setup.MaxRPM) ? 0.f : Key.Time / Setup.MaxRPM;
		const float ValueFloat = FMath::IsNearlyZero(PeakTorque) ? 0.f : Key.Value / PeakTorque;
		PxSetup.mTorqueCurve.addPair(FMath::Clamp(KeyFloat, 0.f, 1.f), FMath::Clamp(ValueFloat, 0.f, 1.f)); // Normalize torque to 0-1 range
	}
}

// Copy UWheeledVehicleMovementComponent4W GetVehicleGearSetup()
static void GetVehicleGearSetup(const FTankVehicleTransmissionData& Setup, PxVehicleGearsData& PxSetup)
{
	PxSetup.mSwitchTime = Setup.GearSwitchTime;
	PxSetup.mRatios[PxVehicleGearsData::eREVERSE] = Setup.ReverseGearRatio;
	for (int32 i = 0; i < Setup.ForwardGears.Num(); i++)
	{
		PxSetup.mRatios[i + PxVehicleGearsData::eFIRST] = Setup.ForwardGears[i].Ratio;
	}
	PxSetup.mFinalRatio = Setup.FinalRatio;
	PxSetup.mNbRatios = Setup.ForwardGears.Num() + PxVehicleGearsData::eFIRST;
}

// Copy UWheeledVehicleMovementComponent4W GetVehicleGearSetup()
static void GetVehicleAutoBoxSetup(const FTankVehicleTransmissionData& Setup, PxVehicleAutoBoxData& PxSetup)
{
	for (int32 i = 0; i < Setup.ForwardGears.Num(); i++)
	{
		const FTankVehicleGearData& GearData = Setup.ForwardGears[i];
		PxSetup.mUpRatios[i + PxVehicleGearsData::eFIRST] = GearData.UpRatio;
		PxSetup.mDownRatios[i + PxVehicleGearsData::eFIRST] = GearData.DownRatio;
	}
	PxSetup.mUpRatios[PxVehicleGearsData::eNEUTRAL] = Setup.NeutralGearUpRatio;
	PxSetup.setLatency(Setup.GearAutoBoxLatency);
}

static void SetupDriveHelper(const UTankVehicleMovementComponent* VehicleData, const PxVehicleWheelsSimData* PWheelsSimData, PxVehicleDriveSimData& DriveData)
{
	PxVehicleEngineData EngineSetup;
	GetVehicleEngineSetup(VehicleData->EngineSetup, EngineSetup);
	DriveData.setEngineData(EngineSetup);

	PxVehicleClutchData ClutchSetup;
	ClutchSetup.mStrength = M2ToCm2(VehicleData->TransmissionSetup.ClutchStrength);
	DriveData.setClutchData(ClutchSetup);

	PxVehicleGearsData GearSetup;
	GetVehicleGearSetup(VehicleData->TransmissionSetup, GearSetup);
	DriveData.setGearsData(GearSetup);

	PxVehicleAutoBoxData AutoBoxSetup;
	GetVehicleAutoBoxSetup(VehicleData->TransmissionSetup, AutoBoxSetup);
	DriveData.setAutoBoxData(AutoBoxSetup);
}

#endif // WITH_PHYSX 

#if WITH_PHYSX_VEHICLES

void UTankVehicleMovementComponent::SetupVehicle()
{
	// Setup the chassis and wheel shapes
	SetupVehicleShapes();

	// Setup mass properties
	SetupVehicleMass();

	PxVehicleWheelsSimData* WheelsSimData = PxVehicleWheelsSimData::allocate(WheelSetups.Num());
	SetupWheels(WheelsSimData);

	// Setup drive data
	PxVehicleDriveSimData DriveData;
	SetupDriveHelper(this, WheelsSimData, DriveData);

	// Create the vehicle
	PxVehicleDriveTank* VehicleDriveTank = PxVehicleDriveTank::allocate(WheelSetups.Num());
	check(VehicleDriveTank);

	VehicleDriveTank->setDriveModel(PxVehicleDriveTankControlModel::eSPECIAL);

	FPhysicsCommand::ExecuteWrite(UpdatedPrimitive->GetBodyInstance()->GetPhysicsActorHandle(), [&](const FPhysicsActorHandle& Actor)
		{

#if WITH_IMMEDIATE_PHYSX
			PxRigidActor* PRigidActor = nullptr;
#else
			PxRigidActor* PRigidActor = Actor.SyncActor;
#endif

			if (PRigidActor)
			{
				if (PxRigidDynamic* PRigidDynamic = PRigidActor->is<PxRigidDynamic>())
				{
					VehicleDriveTank->setup(GPhysXSDK, FPhysicsInterface::GetPxRigidDynamic_AssumesLocked(UpdatedPrimitive->GetBodyInstance()->GetPhysicsActorHandle()), *WheelsSimData, DriveData, WheelSetups.Num());
					VehicleDriveTank->setToRestState();

					// cleanup
					WheelsSimData->free();
				}
			}
		});

	// cache values
	PVehicle = VehicleDriveTank;
	PVehicleDrive = VehicleDriveTank;

	SetUseAutoGears(TransmissionSetup.bUseGearAutoBox);
}

void UTankVehicleMovementComponent::UpdateSimulation(float DeltaTime)
{
	if (PVehicleDrive == NULL) { return; }

	FPhysicsCommand::ExecuteWrite(UpdatedPrimitive->GetBodyInstance()->GetPhysicsActorHandle(), [&](const FPhysicsActorHandle& Actor)
		{
			
		});
}

#endif // WITH_PHYSX_VEHICLES

void UTankVehicleMovementComponent::UpdateEngineSetup(const FTankVehicleEngineData& NewEngineSetup)
{
#if PHYSICS_INTERFACE_PHYSX
	if (PVehicleDrive)
	{
		PxVehicleEngineData EngineData;
		GetVehicleEngineSetup(NewEngineSetup, EngineData);

		PxVehicleDriveTank* VehicleDriveTank = static_cast<PxVehicleDriveTank*>(PVehicleDrive);
		VehicleDriveTank->mDriveSimData.setEngineData(EngineData);
	}
#endif // WITH_PHYSX
}

void UTankVehicleMovementComponent::UpdateTransmissionSetup(const FTankVehicleTransmissionData& NewGearSetup)
{
#if PHYSICS_INTERFACE_PHYSX
	if (PVehicleDrive)
	{
		PxVehicleGearsData GearData;
		GetVehicleGearSetup(NewGearSetup, GearData);

		PxVehicleAutoBoxData AutoBoxData;
		GetVehicleAutoBoxSetup(NewGearSetup, AutoBoxData);

		PxVehicleDriveTank* VehicleDriveTank = static_cast<PxVehicleDriveTank*>(PVehicleDrive);
		VehicleDriveTank->mDriveSimData.setGearsData(GearData);
		VehicleDriveTank->mDriveSimData.setAutoBoxData(AutoBoxData);
	}
#endif // WITH_PHYSX
}

// Copy UWheeledVehicleMovementComponent4W::BackwardsConvertCm2ToM2()
static void BackwardsConvertCm2ToM2(float& val, float defaultValue)
{
	if (val != defaultValue)
	{
		val = Cm2ToM2(val);
	}
}

// Copy UWheeledVehicleMovementComponent4W::Serialize()
void UTankVehicleMovementComponent::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
#if WITH_PHYSX
	if (Ar.IsLoading() && Ar.UE4Ver() < VER_UE4_VEHICLES_UNIT_CHANGE)
	{
		PxVehicleEngineData DefEngineData;
		float DefaultRPM = OmegaToRPM(DefEngineData.mMaxOmega);

		//we need to convert from old units to new. This backwards compatable code fails in the rare case that they were using very strange values that are the new defaults in the correct units.
		EngineSetup.MaxRPM = EngineSetup.MaxRPM != DefaultRPM ? OmegaToRPM(EngineSetup.MaxRPM) : DefaultRPM;	//need to convert from rad/s to RPM
	}

	if (Ar.IsLoading() && Ar.UE4Ver() < VER_UE4_VEHICLES_UNIT_CHANGE2)
	{
		PxVehicleEngineData DefEngineData;
		PxVehicleClutchData DefClutchData;

		//we need to convert from old units to new. This backwards compatable code fails in the rare case that they were using very strange values that are the new defaults in the correct units.
		BackwardsConvertCm2ToM2(EngineSetup.DampingRateFullThrottle, DefEngineData.mDampingRateFullThrottle);
		BackwardsConvertCm2ToM2(EngineSetup.DampingRateZeroThrottleClutchDisengaged, DefEngineData.mDampingRateZeroThrottleClutchDisengaged);
		BackwardsConvertCm2ToM2(EngineSetup.DampingRateZeroThrottleClutchEngaged, DefEngineData.mDampingRateZeroThrottleClutchEngaged);
		BackwardsConvertCm2ToM2(EngineSetup.MOI, DefEngineData.mMOI);
		BackwardsConvertCm2ToM2(TransmissionSetup.ClutchStrength, DefClutchData.mStrength);
	}
#endif // WITH_PHYSX
}

// Copy UWheeledVehicleMovementComponent4W::ComputeConstants()
void UTankVehicleMovementComponent::ComputeConstants()
{
	Super::ComputeConstants();
	MaxEngineRPM = EngineSetup.MaxRPM;
}

PRAGMA_ENABLE_DEPRECATION_WARNINGS
