// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnBox.generated.h"

class UBoxComponent;


/**
* Spawns actors and pawns within box. Can use multiple boxes in different areas or one over the world,
* which will trace down to surface level to spawn actors on the surface.
*/
UCLASS()
class UE4_BATTLETANK_API ASpawnBox : public AActor
{
	GENERATED_BODY()
	
private:
	/** Spawn area */
	UPROPERTY(VisibleDefaultsOnly)
	UBoxComponent * SpawnVolume;

	/** Location to spawn at */
	FVector TargetLocation;

	/** Rotation of terrain */
	FRotator TargetRotation;

	/** Radius to search */
	float SearchRadius;

public:
	/** Sets default values for this actor's properties */
	ASpawnBox();

protected:
	/** Spawns actors inside box, at random empty locations */
	bool PlaceActor(TSubclassOf<AActor> ToSpawn, float Radius);

	/** Spawns pawns inside box, at random empty locations */
	bool PlacePawn(TSubclassOf<APawn> ToSpawn, float Radius);

private:
	/** Generates random positions */
	template<class T>
	bool RandomlyPlaceActors(TSubclassOf<T> ToSpawn);

	/** Returns true if location is empty */
	bool FindEmptyLocation(FVector & OutLocation);

	/** Checks if selected location is empty. Can check between box boundries or surface below if wanting to spawn actor directly on surface */
	bool CanSpawnAtLocation(FVector Location, bool bCheckSurfaceBelow);

	/** Spawns actors inside box, at random empty locations */
	bool SpawnActor(TSubclassOf<AActor> ToSpawn);

	/** Spawns pawns inside box, at random empty locations */
	bool SpawnActor(TSubclassOf<APawn> ToSpawn);

	/** Find floor location under spawned actor */
	bool FindFloorLocation(FVector & OutLocation, FVector & OutRotation);

};
