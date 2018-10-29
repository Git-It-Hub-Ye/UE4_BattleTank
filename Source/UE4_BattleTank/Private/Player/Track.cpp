// Copyright 2018 Stuart McDonald.

#include "Track.h"
#include "SpawnPoint.h"
#include "WheelSuspension.h"


UTrack::UTrack()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTrack::SetThrottle(float Throttle)
{
	float CurrentThrottle = FMath::Clamp<float>(Throttle, -1, +1);
	DriveTrack(CurrentThrottle);
}

void UTrack::DriveTrack(float CurrentThrottle)
{
	auto ForceApplied = CurrentThrottle * TrackMaxDrivingForce;
	// Get force to apply to each wheel
	auto Wheels = GetWheels();
	auto ForcePerWheel = ForceApplied / Wheels.Num();

	for (AWheelSuspension * Wheel : Wheels)
	{
		Wheel->AddDrivingForce(ForcePerWheel);
	}
}

TArray<AWheelSuspension*> UTrack::GetWheels() const
{
	TArray<AWheelSuspension*> WheelArray;

	// Set array of all child components of track
	TArray<USceneComponent*> Children;
	GetChildrenComponents(true, Children);

	// Get all spawn point components
	for (USceneComponent* Child : Children)
	{
		auto SpawnPointChild = Cast<USpawnPoint>(Child);
		if (!SpawnPointChild) { continue; }

		AActor * SpawnedActor = SpawnPointChild->GetSpawnedActor();
		auto Wheel = Cast<AWheelSuspension>(SpawnedActor);
		if (!Wheel) { continue; }
		WheelArray.Add(Wheel);
	}

	return WheelArray;
}
