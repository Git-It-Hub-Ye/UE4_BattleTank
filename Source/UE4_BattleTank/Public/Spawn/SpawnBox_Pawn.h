// Copyright 2018 - 2021 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "Spawn/SpawnBox.h"
#include "SpawnBox_Pawn.generated.h"

/**
 * Spawns pawns within box (eg - AI Bots or Players). Can use multiple boxes in different areas or one over the world,
* which will trace down to surface level to spawn actors on the surface.
 */
UCLASS()
class UE4_BATTLETANK_API ASpawnBox_Pawn : public ASpawnBox
{
	GENERATED_BODY()
	
public:
	/** Try place pawn */
	bool PlacePawns(TSubclassOf<APawn> ToSpawn, float Radius);
	
};
