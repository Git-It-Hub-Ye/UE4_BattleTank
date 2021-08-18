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

	ApplyBrakes(bBrakesApplied);

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
	MoveForwardValue = FMath::Clamp<float>(Value, -1, 1);
	float ForwardSpeed = MoveForwardValue * ForwardMovementRate * GetWorld()->GetDeltaSeconds();

	if (Value != 0)
	{
		if (GetOwner() == NULL) { return; }
		GetOwner()->AddActorLocalOffset(FVector(ForwardSpeed, 0.f, 0.f), false, nullptr, ETeleportType::TeleportPhysics);
	}

	MovementValuesForAnimation();
}

void UTankAISimpleMovementComp::IntendTurnRight(float Value)
{
	TurnRightValue = FMath::Clamp<float>(Value, -1, 1);

	if (GetOwner() == NULL) { return; }
	float TurningSpeed = TurnRightValue * TurnRate * GetWorld()->DeltaTimeSeconds;

	if (Value != 0)
	{
		GetOwner()->AddActorLocalRotation(FRotator(0.f, TurningSpeed, 0.f), false, nullptr, ETeleportType::TeleportPhysics);
	}
}

void UTankAISimpleMovementComp::ApplyBrakes(bool bApplyBrakes)
{
	for (int32 i = 0; i < WheelSetups.Num(); i++)
	{
		SetBrakeTorque(5000.f, i);
	}
}

void UTankAISimpleMovementComp::MovementValuesForAnimation()
{
	if (TankOwner != NULL)
	{
		TankOwner->ApplyInputAnimationValues(TurnRate, TurnSpeed);
	}
}

