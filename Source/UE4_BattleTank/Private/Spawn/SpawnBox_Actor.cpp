// Copyright 2018 - 2022 Stuart McDonald.

#include "SpawnBox_Actor.h"

bool ASpawnBox_Actor::PlaceActors(TSubclassOf<AActor> ToSpawn, float Radius)
{
	if (ToSpawn)
	{
		return PlaceActor(ToSpawn, Radius);
	}
	return false;
}

