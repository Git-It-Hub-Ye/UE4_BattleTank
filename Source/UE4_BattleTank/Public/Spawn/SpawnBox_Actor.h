// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "Spawn/SpawnBox.h"
#include "SpawnBox_Actor.generated.h"

/**
 * Spawns actors within box (eg - triggers, intractable actors or props). Can use multiple boxes in different areas or one over the world,
* which will trace down to surface level to spawn actors on the surface.
 */
UCLASS()
class UE4_BATTLETANK_API ASpawnBox_Actor : public ASpawnBox
{
	GENERATED_BODY()

private:
	/** Actors to spawn */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TArray<TSubclassOf<AActor>> SpawnActorArray;

	/** Initial amount of each actor in array to spawn */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	float InitialSpawnCount;
	
public:
	ASpawnBox_Actor();

	bool PlaceActors();

protected:
	virtual void BeginPlay() override;

private:
	void PlaceInitialActors();
	
};
