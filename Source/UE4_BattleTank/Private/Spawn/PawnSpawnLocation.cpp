// Copyright 2018 Stuart McDonald.

#include "PawnSpawnLocation.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

APawnSpawnLocation::APawnSpawnLocation()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SpawnVolume = CreateDefaultSubobject<UBoxComponent>(FName("Spawn Box"));
	SetRootComponent(SpawnVolume);

	SearchRadius = 100.f;
}

void APawnSpawnLocation::BeginPlay()
{
	Super::BeginPlay();
	TrySpawnActor();
}

bool APawnSpawnLocation::TrySpawnActor()
{
	if (BPSpawnAIPawn)
	{
		PlaceSpawnActor();
	}
	return true;
}

void APawnSpawnLocation::PlaceSpawnActor()
{
	bool found = FindEmptyLocation(SpawnPosition.Location);
	if (found)
	{
		PlaceActor(BPSpawnAIPawn);
	}
}

bool APawnSpawnLocation::FindEmptyLocation(FVector & OutLocation)
{
	FBox Bounds(SpawnVolume->GetScaledBoxExtent() * -1, SpawnVolume->GetScaledBoxExtent());

	const int32 MAX_ATTEMPTS = 10;
	for (size_t i = 0; i < MAX_ATTEMPTS; i++)
	{
		FVector CandidatePoint = FMath::RandPointInBox(Bounds);
		if (CanSpawnAtLocation(CandidatePoint, SearchRadius))
		{
			OutLocation = CandidatePoint;
			return true;
		}
	}
	return false;
}

bool APawnSpawnLocation::CanSpawnAtLocation(FVector Location, float Radius)
{
	FHitResult HitResult;
	FVector GlobalLocation = ActorToWorld().TransformPosition(Location);
	bool HasHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		GlobalLocation,
		GlobalLocation,
		FQuat::Identity,
		ECollisionChannel::ECC_WorldDynamic,
		FCollisionShape::MakeSphere(Radius)
	);

	return !HasHit;
}

void APawnSpawnLocation::PlaceActor(TSubclassOf<APawn> ToSpawn)
{
	APawn * Spawned = GetWorld()->SpawnActor<APawn>(ToSpawn, SpawnPosition.Location, FRotator(0.f,0.f,0.f));

	if (Spawned == nullptr) { return; }
	Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	Spawned->SpawnDefaultController();
}

