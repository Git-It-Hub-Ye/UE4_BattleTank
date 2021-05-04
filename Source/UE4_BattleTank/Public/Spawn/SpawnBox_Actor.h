// Copyright 2018 - 2021 Stuart McDonald.

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

public:
	/** Try place actor */
	bool PlaceActors(TSubclassOf<AActor> ToSpawn, float Radius);

};
