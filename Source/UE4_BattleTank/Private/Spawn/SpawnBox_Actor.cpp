// Copyright 2018 Stuart McDonald.

#include "SpawnBox_Actor.h"

bool ASpawnBox_Actor::PlaceActors(TSubclassOf<AActor> ToSpawn, float SearchRadius)
{
	if (ToSpawn)
	{
		return PlaceActor(ToSpawn, SearchRadius);
	}
	return false;
}

