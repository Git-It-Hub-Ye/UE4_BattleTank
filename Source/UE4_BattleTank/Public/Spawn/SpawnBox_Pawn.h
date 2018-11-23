// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "Spawn/SpawnBox.h"
#include "SpawnBox_Pawn.generated.h"

/**
 * Spawns pawns within box (eg - AI Bots or Players)
 */
UCLASS()
class UE4_BATTLETANK_API ASpawnBox_Pawn : public ASpawnBox
{
	GENERATED_BODY()
	
public:
	bool PlacePawns(TSubclassOf<APawn> ToSpawn, float SearchRadius);
	
};
