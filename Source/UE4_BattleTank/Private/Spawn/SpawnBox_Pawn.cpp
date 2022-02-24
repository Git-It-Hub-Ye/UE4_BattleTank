// Copyright 2018 - 2022 Stuart McDonald.

#include "SpawnBox_Pawn.h"


bool ASpawnBox_Pawn::PlacePawns(TSubclassOf<APawn> ToSpawn, float Radius)
{
	if (ToSpawn)
	{
		return PlacePawn(ToSpawn, Radius);
	}
	return false;
}

