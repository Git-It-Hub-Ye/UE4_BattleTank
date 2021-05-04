// Copyright 2018 to 2021 Stuart McDonald.

#include "SpawnBox_Actor.h"

bool ASpawnBox_Actor::PlaceActors(TSubclassOf<AActor> ToSpawn, float SearchRadius)
{
	if (ToSpawn)
	{
		return PlaceActor(ToSpawn, SearchRadius);
	}
	return false;
}

