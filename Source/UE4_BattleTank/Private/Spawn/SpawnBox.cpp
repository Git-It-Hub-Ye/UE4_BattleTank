// Copyright 2018 - 2021 Stuart McDonald.

#include "SpawnBox.h"
#include "UE4_BattleTank.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

#include "Components/BoxComponent.h"


ASpawnBox::ASpawnBox()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SpawnVolume = CreateDefaultSubobject<UBoxComponent>(FName("Spawn Box"));
	SetRootComponent(SpawnVolume);

	SearchRadius = 100.f;
}

bool ASpawnBox::PlaceActor(TSubclassOf<AActor> ToSpawn, float Radius)
{
	SearchRadius = Radius;
	return RandomlyPlaceActors(ToSpawn);
}

bool ASpawnBox::PlacePawn(TSubclassOf<APawn> ToSpawn, float Radius)
{
	SearchRadius = Radius;
	return RandomlyPlaceActors(ToSpawn);
}

template<class T>
bool ASpawnBox::RandomlyPlaceActors(TSubclassOf<T> ToSpawn)
{
	if (FindEmptyLocation(TargetLocation))
	{
		return SpawnActor(ToSpawn);
	}
	return false;
}

bool ASpawnBox::FindEmptyLocation(FVector & OutLocation)
{
	FBox Bounds(SpawnVolume->GetScaledBoxExtent() * -1, SpawnVolume->GetScaledBoxExtent());

	const int32 MAX_ATTEMPTS = 10;
	for (size_t i = 0; i < MAX_ATTEMPTS; i++)
	{
		FVector CandidatePoint = FMath::RandPointInBox(Bounds);
		if (CanSpawnAtLocation(CandidatePoint, false))
		{
			FVector InitalRotation = FVector(0.f, 0.f, 0.f);
			OutLocation = CandidatePoint;
			if (FindFloorLocation(OutLocation, InitalRotation))
			{
				FRotator Rot = FRotationMatrix::MakeFromZX(InitalRotation, GetActorForwardVector()).Rotator();
				TargetRotation = FRotator(Rot.Pitch, GetActorRotation().Yaw, Rot.Roll);
				return true;
			}
		}
	}
	return false;
}

bool ASpawnBox::CanSpawnAtLocation(FVector Location, bool bCheckSurfaceBelow)
{
	FHitResult HitResult;
	FVector GlobalLocation = bCheckSurfaceBelow ? Location : ActorToWorld().TransformPosition(Location);
	bool HasHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		GlobalLocation,
		GlobalLocation,
		FQuat::Identity,
		TRACE_SPAWN_SWEEP,
		FCollisionShape::MakeSphere(SearchRadius)
	);
	return !HasHit;
}

bool ASpawnBox::FindFloorLocation(FVector & OutLocation, FVector & OutRotation)
{
	FHitResult HitResult;
	FVector StartTrace = ActorToWorld().TransformPosition(OutLocation);
	FVector EndTrace = StartTrace + (FVector(0.f, 0.f, -10000.f));
	FCollisionQueryParams TraceParams;

	if (GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartTrace,
		EndTrace,
		TRACE_SPAWN_TRACE,
		TraceParams)
		)
	{
		OutLocation = HitResult.ImpactPoint;
		OutRotation = HitResult.ImpactNormal;
		return CanSpawnAtLocation(OutLocation, true);
	}
	return false;
}

bool ASpawnBox::SpawnActor(TSubclassOf<AActor> ToSpawn)
{
	FTransform SpawnT(TargetRotation, TargetLocation);
	AActor * Spawned = GetWorld()->SpawnActorDeferred<AActor>(ToSpawn, SpawnT);
	if (Spawned)
	{
		Spawned->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		UGameplayStatics::FinishSpawningActor(Spawned, SpawnT);
		return true;
	}
	return false;
}

bool ASpawnBox::SpawnActor(TSubclassOf<APawn> ToSpawn)
{
	TargetLocation.Z += 150.f;
	FTransform SpawnT(TargetRotation, TargetLocation);
	APawn * Spawned = GetWorld()->SpawnActorDeferred<APawn>(ToSpawn, SpawnT);
	if (Spawned)
	{
		Spawned->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		Spawned->SpawnDefaultController();
		UGameplayStatics::FinishSpawningActor(Spawned, SpawnT);
		return true;
	}
	return false;
}

