// Copyright 2018 Stuart McDonald.

#include "PawnSpawnBox.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"


APawnSpawnBox::APawnSpawnBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SpawnVolume = CreateDefaultSubobject<UBoxComponent>(FName("Spawn Box"));
	SetRootComponent(SpawnVolume);

	SearchRadius = 100.f;
}

void APawnSpawnBox::BeginPlay()
{
	Super::BeginPlay();
	SpawnChosenActor();
}

bool APawnSpawnBox::SpawnChosenActor()
{
	if (BP_ActorToSpawn)
	{
		return PlaceSpawnActor();
	}
	return false;
}

bool APawnSpawnBox::PlaceSpawnActor()
{
	for (size_t i = 0; i < 10; i++)
	{
		bool found = FindEmptyLocation(TargetLocation);
		if (found)
		{
			PlaceActor(BP_ActorToSpawn);
		}
	}
	return false;
}

bool APawnSpawnBox::FindEmptyLocation(FVector & OutLocation)
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

bool APawnSpawnBox::CanSpawnAtLocation(FVector Location, float Radius)
{
	FHitResult HitResult;
	FVector GlobalLocation = ActorToWorld().TransformPosition(Location);
	bool HasHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		GlobalLocation,
		GlobalLocation,
		FQuat::Identity,
		ECollisionChannel::ECC_WorldStatic,
		FCollisionShape::MakeSphere(Radius)
	);

	FColor Colour = HasHit ? FColor::Red : FColor::Green;

	DrawDebugSphere(GetWorld(), GlobalLocation, Radius, 10.f, Colour, true);

	return !HasHit;
}

void APawnSpawnBox::PlaceActor(TSubclassOf<APawn> ToSpawn)
{
	FTransform SpawnT(FRotator(0.f, 0.f, 0.f), TargetLocation);

	APawn * Spawned = GetWorld()->SpawnActorDeferred<APawn>(ToSpawn, SpawnT);
	if (Spawned)
	{
		Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
		UGameplayStatics::FinishSpawningActor(Spawned, SpawnT);
	}
}

