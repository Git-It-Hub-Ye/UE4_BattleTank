// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnBox.generated.h"

class UBoxComponent;

UCLASS()
class UE4_BATTLETANK_API ASpawnBox : public AActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY(VisibleDefaultsOnly)
	UBoxComponent * SpawnVolume;

	FVector TargetLocation;

public:
	/** Sets default values for this actor's properties */
	ASpawnBox();

protected:
	/** Spawns actors inside box, at random empty locations */
	bool PlaceActor(TSubclassOf<AActor> ToSpawn, float SearchRadius);

	/** Spawns pawns inside box, at random empty locations */
	bool PlacePawn(TSubclassOf<APawn> ToSpawn, float SearchRadius);

private:
	/** Generates random positions */
	template<class T>
	bool RandomlyPlaceActors(TSubclassOf<T> ToSpawn, float SearchRadius);

	/** Returns true if location is empty */
	bool FindEmptyLocation(FVector & OutLocation, float SearchRadius);

	/** Checks if selected location is empty */
	bool CanSpawnAtLocation(FVector Location, float Radius);

	/** Spawns actors inside box, at random empty locations */
	void SpawnActor(TSubclassOf<AActor> ToSpawn);

	/** Spawns pawns inside box, at random empty locations */
	void SpawnActor(TSubclassOf<APawn> ToSpawn);

	/** Find floor location under spawned actor */
	bool FindFloorLocation(FVector & OutLocation);

	bool CanSpawnAtFloor(FVector Location, float SearchRadius);
	
};
