// Copyright 2018 - 2021 Stuart McDonald.

#include "Barrel.h"
#include "Engine/World.h"
#include "Tank.h"


void UBarrel::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner() != NULL)
	{
		ATank * OwnerPawn = Cast<ATank>(GetOwner());

		if (OwnerPawn == NULL) { return; }
		OwnerPawn->OnDeath.AddUniqueDynamic(this, &UBarrel::OnOwnerDeath);
	}
}

void UBarrel::OnOwnerDeath()
{
}

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

