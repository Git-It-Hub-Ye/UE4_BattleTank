// Copyright 2018 - 2022 Stuart McDonald.


#include "TankAISimpleMovementComp.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

#include "Components/AudioComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Tank.h"

#if WITH_PHYSX
#include "PhysXPublic.h"
#endif // WITH_PHYSX


UTankAISimpleMovementComp::UTankAISimpleMovementComp()
{
	PrimaryComponentTick.bCanEverTick = false;

	bBrakesApplied = true;
}

void UTankAISimpleMovementComp::BeginPlay()
{
	Super::BeginPlay();

	SetBrakesValue(true);

	if (GetOwner() == NULL) { return; }
	TankOwner = Cast<ATank>(GetOwner());

	if (TankOwner == NULL) { return; }
	TankMesh = Cast<USkeletalMeshComponent>(TankOwner->GetRootComponent());
}


////////////////////////////////////////////////////////////////////////////////
// Drive tank

void UTankAISimpleMovementComp::RequestDirectMove(const FVector& MoveVelocity, bool bForceMaxSpeed)
{
	// No need for super, as we are replacing the functionality.
	auto TankForwardDirection = GetOwner()->GetActorForwardVector().GetSafeNormal();
	auto AIForwardIntention = MoveVelocity.GetSafeNormal();

	auto ForwardThrow = FVector::DotProduct(TankForwardDirection, AIForwardIntention);
	IntendMoveForward(ForwardThrow);

	auto RightThrow = FVector::CrossProduct(TankForwardDirection, AIForwardIntention).Z;
	IntendTurnRight(RightThrow);
}

void UTankAISimpleMovementComp::IntendMoveForward(float Value)
{
	float MoveForwardValue = FMath::Clamp<float>(Value, -0.5, 1.f);
	WheelTorque = MoveForwardValue * ForcePerWheel * GetWorld()->GetDeltaSeconds();

	if (Value != 0 && Value != CurrentForwardValue)
	{
		CurrentForwardValue = Value;

		if (TankOwner == NULL) { return; }
		for (int32 i = 0; i < WheelSetups.Num(); i++)
		{
			SetDriveTorque(WheelTorque, i);
		}
	}

	MovementValuesForAnimation();
}

void UTankAISimpleMovementComp::IntendTurnRight(float Value)
{
	float TurnRightValue = FMath::Clamp<float>(Value, -1, 1);
	float TurnTorque = TurnRightValue * TurnForce * GetWorld()->GetDeltaSeconds();

	if (Value != 0 && CurrentForwardValue != 0)
	{
		if (TankMesh == NULL) { return; }
		TankMesh->AddTorqueInRadians(FVector(0.f, 0.f, TurnTorque), FName("T-62_body"), true);
	}
}

void UTankAISimpleMovementComp::SetBrakesValue(bool bSetBrake)
{
	float BrakeValue = bSetBrake ? BrakeTorque : 0.f;
	CurrentForwardValue = 0;

	for (int32 i = 0; i < WheelSetups.Num(); i++)
	{
		SetBrakeTorque(BrakeValue, i);
	}
}


////////////////////////////////////////////////////////////////////////////////
// Drive data

void UTankAISimpleMovementComp::MovementValuesForAnimation()
{
	if (TankOwner != NULL)
	{
		TankOwner->ApplyInputAnimationValues(GetRightWheelSpeed(), GetLeftWheelSpeed());
	}
}

float UTankAISimpleMovementComp::GetLeftWheelSpeed()
{
#if WITH_PHYSX_VEHICLES

	if (PVehicle && Wheels.Num() > 0)
	{
		float RotationSpeed = PVehicle->mWheelsDynData.getWheelRotationSpeed(0);
		return FMath::RoundToFloat(RotationSpeed);
	}

#endif // WITH_PHYSX

	return 0.f;
}

float UTankAISimpleMovementComp::GetRightWheelSpeed()
{
#if WITH_PHYSX_VEHICLES

	if (PVehicle && Wheels.Num() > 1)
	{
		float RotationSpeed = PVehicle->mWheelsDynData.getWheelRotationSpeed(1);

		return FMath::RoundToFloat(RotationSpeed);
	}

#endif // WITH_PHYSX

	return 0.f;
}

