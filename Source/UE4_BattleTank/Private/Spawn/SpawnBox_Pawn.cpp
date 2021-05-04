// Copyright 2018 to 2021 Stuart McDonald.

#include "SpawnBox_Pawn.h"


bool ASpawnBox_Pawn::PlacePawns(TSubclassOf<APawn> ToSpawn, float SearchRadius)
{
	if (ToSpawn)
	{
		return PlacePawn(ToSpawn, SearchRadius);
	}
	return false;
}

