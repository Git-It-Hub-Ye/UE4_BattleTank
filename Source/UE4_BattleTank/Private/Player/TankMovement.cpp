// Copyright 2018 - 2022 Stuart McDonald.

#include "TankMovement.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

#include "Components/AudioComponent.h"
#include "Tank.h"


UTankMovement::UTankMovement()
{
	PrimaryComponentTick.bCanEverTick = false;

	ForwardMovementRate = 100.f;
	TurnRate = 0.5f;
	bBrakesApplied = false;
}

void UTankMovement::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetOwner() == NULL) { return; }
	TankOwner = Cast<ATank>(GetOwner());
}


////////////////////////////////////////////////////////////////////////////////
// Drive tank

void UTankMovement::RequestDirectMove(const FVector & MoveVelocity, bool bForceMaxSpeed)
{
	// No need for super, as we are replacing the functionality.
	auto TankForwardDirection = GetOwner()->GetActorForwardVector().GetSafeNormal();
	auto AIForwardIntention = MoveVelocity.GetSafeNormal();

	auto ForwardThrow = FVector::DotProduct(TankForwardDirection, AIForwardIntention);
	IntendMoveForward(ForwardThrow);

	auto RightThrow = FVector::CrossProduct(TankForwardDirection, AIForwardIntention).Z;
	IntendTurnRight(RightThrow);
}

void UTankMovement::IntendMoveForward(float Value)
{
	MoveForwardValue = FMath::Clamp<float>(Value, -1, 1);
	float ForwardSpeed = MoveForwardValue * ForwardMovementRate * GetWorld()->GetDeltaSeconds();

	if (Value != 0)
	{
		if (GetOwner() == NULL) { return; }

		if (bBrakesApplied == true) { ApplyBrakes(false); }
		GetOwner()->AddActorLocalOffset(FVector(ForwardSpeed, 0.f, 0.f));
	}

	MovementValuesForAnimation();
}

void UTankMovement::IntendTurnRight(float Value)
{
	TurnRightValue = FMath::Clamp<float>(Value, -1, 1);

	if (Value != 0.f)
	{
		if (GetOwner() == NULL) { return; }

		float TurnValue = FMath::Clamp<float>(Value, -1, 1);

		float MaxSpeed = FGenericPlatformMath::Abs<float>(TurnValue * TurnRate);
		float MaxTurnSpeed = TurnValue * TurnRate;

		TurnSpeed += MaxTurnSpeed * GetWorld()->DeltaTimeSeconds;
		TurnSpeed = FMath::Clamp<float>(TurnSpeed, -MaxSpeed, MaxSpeed);

		float TurningSpeed = FMath::Clamp<float>(TurnSpeed, -TurnRate, TurnRate);
		GetOwner()->AddActorLocalRotation(FRotator(0.f, TurningSpeed, 0.f));
	}
}

void UTankMovement::ApplyBrakes(bool bApplyBrakes)
{
	if (WheelSetups.Num() > 0)
	{
		/*float TorqueApplied = 0.f;
		if (bApplyBrakes == true)
		{
			TorqueApplied = BrakeTorquePerWheel;
			bBrakesApplied = true;
		}
		else
		{
			bBrakesApplied = false;
		}
		for (int32 i = 0; i < WheelSetups.Num(); i++)
		{
			SetBrakeTorque(TorqueApplied, i);
		}*/
	}
}

void UTankMovement::MovementValuesForAnimation()
{
	if (TankOwner != NULL)
	{
		//TankOwner->ApplyInputMovementBehaviours(TurnRate, TurnSpeed);
	}
}

