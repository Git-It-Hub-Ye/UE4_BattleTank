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
	this->OnComponentBeginOverlap.AddDynamic(this, &UTrack::OnOverlap);
	this->OnComponentEndOverlap.AddDynamic(this, &UTrack::OnEndOverlap);
}

void UTrack::OnOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor)
	{
		OverlappedActors++;
	}
}

void UTrack::OnEndOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		OverlappedActors--;
	}
}

void UTrack::SetThrottle(float Throttle)
{
	if (OverlappedActors > 0)
	{
		auto ForceApplied = GetForwardVector() * Throttle * TrackMaxDrivingForce;
		auto ForceLocation = GetComponentLocation();
		auto TankRoot = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());

		if (!TankRoot) { return; }
		TankRoot->AddForceAtLocation(ForceApplied, ForceLocation);
	}
}

