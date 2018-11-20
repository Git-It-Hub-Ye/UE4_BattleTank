// Copyright 2018 Stuart McDonald.

#include "BattleTankGameModeBase.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Player/PawnSpawnBox.h"
#include "EngineUtils.h"

ABattleTankGameModeBase::ABattleTankGameModeBase()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Dynamic/Tank/Behaviour/BP_PlayerTank"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	static ConstructorHelpers::FClassFinder<APawn> AIBotPawnBPClass(TEXT("/Game/Dynamic/Tank/Behaviour/BP_AITank"));
	if (AIBotPawnBPClass.Class != NULL)
	{
		DefaultAIBotClass = AIBotPawnBPClass.Class;
	}

	TimeBetweenWaves = 5.f;
	NumOfBotsToSpawn = 1;
	MaxBotsThisRound = 0;
	MaxBotsAtOnce = 10;
	TotalBotsSpawned = 0;
	CurrentNumOfBotsAlive = 0;
	CurrentWave = 0;
}

void ABattleTankGameModeBase::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void ABattleTankGameModeBase::StartPlay()
{
	Super::StartPlay();
	GetSpawnLocations();
	PrepareNewWave();
}

void ABattleTankGameModeBase::GetSpawnLocations()
{
	for (TActorIterator<APawnSpawnBox> i(GetWorld()); i; ++i)
	{
		APawnSpawnBox * SpawnBox = Cast<APawnSpawnBox>(*i);
		if (SpawnBox)
		{
			SpawnBoxArray.Add(SpawnBox);
		}
	}
}

void ABattleTankGameModeBase::PrepareNewWave()
{
	CurrentWave++;
	TotalBotsSpawned = 0;
	CurrentNumOfBotsAlive = 0;
	MaxBotsThisRound = NumOfBotsToSpawn * CurrentWave;
	GetWorldTimerManager().SetTimer(NextWaveHandle, this, &ABattleTankGameModeBase::StartWave, TimeBetweenWaves, false);
}

void ABattleTankGameModeBase::StartWave()
{
	GetWorldTimerManager().ClearTimer(NextWaveHandle);
	if (TotalBotsSpawned < MaxBotsThisRound)
	{
		SpawnNewAIPawn();
	}
	else if (CurrentNumOfBotsAlive <= 0)
	{
		EndWave();
	}
}

void ABattleTankGameModeBase::EndWave()
{
	PrepareNewWave();
}

void ABattleTankGameModeBase::SpawnNewAIPawn()
{
	if (SpawnBoxArray.Num() > 0)
	{
		int32 FailedSpawns = 0;
		for (CurrentNumOfBotsAlive; CurrentNumOfBotsAlive < MaxBotsAtOnce; CurrentNumOfBotsAlive)
		{
			int32 RandNum = FMath::RandRange(0, SpawnBoxArray.Num() - 1);
			APawnSpawnBox * SpawnBox = SpawnBoxArray[RandNum];
			if (SpawnBox->SpawnChosenActor(DefaultAIBotClass, 500.f))
			{
				CurrentNumOfBotsAlive++;
				TotalBotsSpawned++;
				if (TotalBotsSpawned >= MaxBotsThisRound) { break; }
			}
			else
			{
				FailedSpawns++;
				if (FailedSpawns >= 10) { break; }
			}
		}
	}
}

void ABattleTankGameModeBase::AIBotDestroyed()
{
	CurrentNumOfBotsAlive--;
	StartWave();
}

