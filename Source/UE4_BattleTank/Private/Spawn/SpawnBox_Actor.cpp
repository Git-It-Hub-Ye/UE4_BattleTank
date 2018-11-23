// Copyright 2018 Stuart McDonald.

#include "SpawnBox_Actor.h"


ASpawnBox_Actor::ASpawnBox_Actor()
{
	InitialSpawnCount = 1;
}

void ASpawnBox_Actor::BeginPlay()
{
	Super::BeginPlay();
	PlaceInitialActors();
}

void ASpawnBox_Actor::PlaceInitialActors()
{
	if (SpawnActorArray.Num() && InitialSpawnCount > 0)
	{
		for (int32 i = 0; i < SpawnActorArray.Num(); i++)
		{
			if (SpawnActorArray[i])
			{
				for (int32 j = 0; j < InitialSpawnCount; j++)
				{
					PlaceActor(SpawnActorArray[i], 500.f);
				}
			}
		}
	}
}

bool ASpawnBox_Actor::PlaceActors()
{
	if (SpawnActorArray.Num() > 0)
	{
		int32 RandNum = FMath::RandRange(0, SpawnActorArray.Num() - 1);
		if (SpawnActorArray[RandNum])
		{
			return PlaceActor(SpawnActorArray[RandNum], 500.f);
		}
	}
	return false;
}

