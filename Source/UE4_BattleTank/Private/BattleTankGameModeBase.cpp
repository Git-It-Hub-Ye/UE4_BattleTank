// Copyright 2018 Stuart McDonald.

#include "BattleTankGameModeBase.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Spawn/SpawnBox.h"
#include "Spawn/SpawnBox_Pawn.h"
#include "Spawn/SpawnBox_Actor.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "Player/Tank.h"

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

void ABattleTankGameModeBase::Logout(AController * Exiting)
{
}

void ABattleTankGameModeBase::StartPlay()
{
	Super::StartPlay();
	GetSpawnLocations();
	PrepareNewWave();
}

void ABattleTankGameModeBase::GetSpawnLocations()
{
	for (TActorIterator<ASpawnBox> i(GetWorld()); i; ++i)
	{
		ASpawnBox_Pawn * BotSpawnBox = Cast<ASpawnBox_Pawn>(*i);
		if (BotSpawnBox)
		{
			BotSpawnBoxArray.Add(BotSpawnBox);
		}

		ASpawnBox_Actor * ActorSpawnBox = Cast<ASpawnBox_Actor>(*i);
		if (ActorSpawnBox)
		{
			ActorSpawnBoxArray.Add(ActorSpawnBox);
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
	if (MaxBotsThisRound > 0 && TotalBotsSpawned < MaxBotsThisRound)
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
	if (BotSpawnBoxArray.Num() > 0)
	{
		int32 FailedSpawns = 0;
		for (CurrentNumOfBotsAlive; CurrentNumOfBotsAlive < MaxBotsAtOnce; CurrentNumOfBotsAlive)
		{
			int32 RandNum = FMath::RandRange(0, BotSpawnBoxArray.Num() - 1);
			ASpawnBox_Pawn * SpawnBox = BotSpawnBoxArray[RandNum];
			if (SpawnBox->PlacePawns(DefaultAIBotClass, 500.f))
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

void ABattleTankGameModeBase::AIBotDestroyed(APawn * AIPawn)
{
	if (AIPawn && !AIPawn->IsPlayerControlled())
	{
		CurrentNumOfBotsAlive--;
		StartWave();
	}
}

void ABattleTankGameModeBase::PlayerDestroyed()
{
	for (FConstPlayerControllerIterator i = GetWorld()->GetPlayerControllerIterator(); i; i++)
	{
		APlayerController * PC = i->Get();
		if (PC && PC->GetPawn())
		{
			ATank * PlayerPawn = Cast<ATank>(PC->GetPawn());

			// If a player is not destroyed, then stop function
			if (!PlayerPawn->IsTankDestroyed()) { return; }
		}
	}

	// Only runs if all players are destroyed
	GameOver();
}

void ABattleTankGameModeBase::GameOver()
{
	UE_LOG(LogTemp, Warning, TEXT("Game Over"))
}

