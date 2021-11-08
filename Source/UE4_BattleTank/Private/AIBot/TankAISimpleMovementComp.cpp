// Copyright 2018 - 2021 Stuart McDonald.


#include "TankAISimpleMovementComp.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

#include "Components/AudioComponent.h"
#include "Tank.h"


UTankAISimpleMovementComp::UTankAISimpleMovementComp()
{
	PrimaryComponentTick.bCanEverTick = false;

	ForwardMovementRate = 100.f;
	TurnRate = 30.f;
	bBrakesApplied = true;
}

void UTankAISimpleMovementComp::BeginPlay()
{
	Super::BeginPlay();

	SetBrakesValue(true);

	if (GetOwner() == NULL) { return; }
	TankOwner = Cast<ATank>(GetOwner());
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
	float MaxSpeed = FGenericPlatformMath::Abs<float>(MoveForwardValue * ForwardMovementRate);

	float ForwardSpeed  = MoveForwardValue * ForwardMovementRate;
	CurrentForwardSpeed += ForwardSpeed * GetWorld()->GetDeltaSeconds();
	CurrentForwardSpeed = FMath::Clamp<float>(CurrentForwardSpeed, -MaxSpeed, MaxSpeed);

	if (Value != 0)
	{
		if (GetOwner() == NULL) { return; }
		GetOwner()->AddActorLocalOffset(FVector(CurrentForwardSpeed, 0.f, 0.f), false, nullptr, ETeleportType::TeleportPhysics);
	}

	MovementValuesForAnimation();
}

void UTankAISimpleMovementComp::IntendTurnRight(float Value)
{
	float TurnRightValue = FMath::Clamp<float>(Value, -1, 1);
	float MaxSpeed = FGenericPlatformMath::Abs<float>(TurnRightValue * TurnRate);

	float TurningSpeed = TurnRightValue * TurnRate;
	CurrentTurningSpeed += TurningSpeed * GetWorld()->DeltaTimeSeconds;
	CurrentTurningSpeed = FMath::Clamp<float>(CurrentTurningSpeed, -MaxSpeed, MaxSpeed);

	if (Value != 0)
	{
		if (GetOwner() == NULL) { return; }
		GetOwner()->AddActorLocalRotation(FRotator(0.f, CurrentTurningSpeed, 0.f), false, nullptr, ETeleportType::TeleportPhysics);
	}
}

void UTankAISimpleMovementComp::SetBrakesValue(bool bSetBrake)
{
	float BrakeValue = bSetBrake ? 5000.f : 0.f;

	for (int32 i = 0; i < WheelSetups.Num(); i++)
	{
		SetBrakeTorque(BrakeValue, i);
	}
}

void UTankAISimpleMovementComp::MovementValuesForAnimation()
{
	if (TankOwner != NULL)
	{
		TankOwner->ApplyInputAnimationValues(CurrentForwardSpeed, CurrentTurningSpeed);
	}
}

