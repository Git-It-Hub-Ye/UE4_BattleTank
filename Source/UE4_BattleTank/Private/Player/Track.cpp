// Copyright 2018 Stuart McDonald.

#include "Track.h"
#include "Engine/StaticMesh.h"


UTrack::UTrack()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTrack::BeginPlay()
{
	Super::BeginPlay();
}

void UTrack::SetThrottle(float Throttle)
{

	auto ForceApplied = GetForwardVector() * Throttle * TrackMaxDrivingForce;
	auto ForceLocation = GetComponentLocation();
	auto TankRoot = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());

	if (!TankRoot) { return; }
	TankRoot->AddForceAtLocation(ForceApplied, ForceLocation);
}

