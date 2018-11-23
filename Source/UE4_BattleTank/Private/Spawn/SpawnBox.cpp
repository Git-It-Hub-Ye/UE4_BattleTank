// Copyright 2018 Stuart McDonald.

#include "SpawnBox.h"
#include "UE4_BattleTank.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"


ASpawnBox::ASpawnBox()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SpawnVolume = CreateDefaultSubobject<UBoxComponent>(FName("Spawn Box"));
	SetRootComponent(SpawnVolume);
}

bool ASpawnBox::PlaceActor(TSubclassOf<AActor> ToSpawn, float SearchRadius)
{
	return RandomlyPlaceActors(ToSpawn, SearchRadius);
}

bool ASpawnBox::PlacePawn(TSubclassOf<APawn> ToSpawn, float SearchRadius)
{
	return RandomlyPlaceActors(ToSpawn, SearchRadius);
}

template<class T>
bool ASpawnBox::RandomlyPlaceActors(TSubclassOf<T> ToSpawn, float SearchRadius)
{
	bool bFound = FindEmptyLocation(TargetLocation, SearchRadius);
	if (bFound)
	{
		SpawnActor(ToSpawn);
	}
	return bFound;
}

bool ASpawnBox::FindEmptyLocation(FVector & OutLocation, float SearchRadius)
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

bool ASpawnBox::CanSpawnAtLocation(FVector Location, float SearchRadius)
{
	FHitResult HitResult;
	FVector GlobalLocation = ActorToWorld().TransformPosition(Location);
	bool HasHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		GlobalLocation,
		GlobalLocation,
		FQuat::Identity,
		ECollisionChannel::ECC_WorldStatic,
		FCollisionShape::MakeSphere(SearchRadius)
	);

	FColor Colour = HasHit ? FColor::Red : FColor::Green;

	DrawDebugSphere(GetWorld(), GlobalLocation, SearchRadius, 10.f, Colour, true);

	return !HasHit;
}

void ASpawnBox::SpawnActor(TSubclassOf<AActor> ToSpawn)
{
	FVector FloorLocation = FVector::ZeroVector;
	if (FindFloorLocation(FloorLocation))
	{
		FTransform SpawnT(FRotator(0.f, 0.f, 0.f), FloorLocation);
		AActor * Spawned = GetWorld()->SpawnActorDeferred<AActor>(ToSpawn, SpawnT);
		if (Spawned)
		{
			Spawned->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
			UGameplayStatics::FinishSpawningActor(Spawned, SpawnT);
		}
	}
}

void ASpawnBox::SpawnActor(TSubclassOf<APawn> ToSpawn)
{
	FTransform SpawnT(FRotator(0.f, 0.f, 0.f), TargetLocation);

	APawn * Spawned = GetWorld()->SpawnActorDeferred<APawn>(ToSpawn, SpawnT);
	if (Spawned)
	{
		Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
		Spawned->SpawnDefaultController();
		UGameplayStatics::FinishSpawningActor(Spawned, SpawnT);
	}
}

bool ASpawnBox::FindFloorLocation(FVector & OutLocation)
{
	FHitResult HitResult;
	FVector StartTrace = ActorToWorld().TransformPosition(TargetLocation);
	FVector EndTrace = StartTrace + (FVector(0.f, 0.f, -10000.f));
	FCollisionQueryParams TraceParams;

	if (GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartTrace,
		EndTrace,
		TRACE_SPAWN,
		TraceParams
	)
		)
	{
		DrawDebugLine(GetWorld(), StartTrace, HitResult.ImpactPoint, FColor::Red, true);
		OutLocation = HitResult.ImpactPoint;
		return CanSpawnAtFloor(HitResult.ImpactPoint, 600.f);
	}
	return false;
}

bool ASpawnBox::CanSpawnAtFloor(FVector Location, float SearchRadius)
{
	FHitResult HitResult;
	bool HasHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		Location,
		Location,
		FQuat::Identity,
		ECollisionChannel::ECC_WorldStatic,
		FCollisionShape::MakeSphere(SearchRadius)
	);

	FColor Colour = HasHit ? FColor::Red : FColor::Green;

	DrawDebugSphere(GetWorld(), Location, SearchRadius, 10.f, Colour, true);

	return !HasHit;
}

