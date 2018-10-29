// Copyright 2018 Stuart McDonald.

#include "Turret.h"
#include "Engine/World.h"

void UTurret::RotateTurret(float RelativeSpeed)
{
	// Move Turret right amount this frame.
	// Given max Rotation speed and the frame rate.

	RelativeSpeed = FMath::Clamp<float>(RelativeSpeed, -1, +1);

	auto RotationChange = RelativeSpeed * MaxDegreesPerSecond * GetWorld()->DeltaTimeSeconds;
	auto NewRotation = RelativeRotation.Yaw + RotationChange;

	SetRelativeRotation(FRotator(0, NewRotation, 0));
}
