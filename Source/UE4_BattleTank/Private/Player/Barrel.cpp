// Copyright 2018 Stuart McDonald.

#include "Barrel.h"
#include "Engine/World.h"

void UBarrel::ElevateBarrel(float RelativeSpeed)
{
	// Move Barrel right amount this frame.
	// Given max elevation speed and the frame rate.

	RelativeSpeed = FMath::Clamp<float>(RelativeSpeed, -1, +1);

	auto ElevationChange = RelativeSpeed * MaxDegreesPerSecond * GetWorld()->DeltaTimeSeconds;
	auto NewRawElevation = RelativeRotation.Pitch + ElevationChange;
	auto Elevation = FMath::Clamp<float>(NewRawElevation, MinElevationDegrees, MaxElevationDegrees);

	SetRelativeRotation(FRotator(Elevation, 0, 0));
}
