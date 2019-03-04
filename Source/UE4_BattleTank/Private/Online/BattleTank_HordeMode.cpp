// Copyright 2018 Stuart McDonald.

#include "BattleTank_HordeMode.h"
#include "UObject/ConstructorHelpers.h"
#include "BattleTankGameState.h"
#include "TankPlayerState.h"
#include "TimerManager.h"
#include "Player/Tank.h"
#include "SpawnBox_Pawn.h"

ABattleTank_HordeMode::ABattleTank_HordeMode()
{
	DefaultPawnAIClass = ATank::StaticClass();

	static ConstructorHelpers::FClassFinder<APawn> AIBotPawnBPClass(TEXT("/Game/Dynamic/Tank/Behaviour/BP_Tank_AI"));
	if (AIBotPawnBPClass.Class != NULL)
	{
		DefaultPawnAIClass = AIBotPawnBPClass.Class;
	}

	CurrentWave = 0;

	bAllowBots = true;
	NumOfBotsToSpawn = 10;
	MaxBotAmountAtOnce = 10;
	MaxBotSpawnAmount = 0;
	TotalBotsSpawned = 0;
	CurrentNumOfBotsAlive = 0;
}

void ABattleTank_HordeMode::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if (NumOfPlayers >= MinPlayersRequired)
	{
		PrepareNewWave();
	}
}


////////////////////////////////////////////////////////////////////////////////
// Handle Waves

void ABattleTank_HordeMode::PrepareNewWave()
{
	CurrentWave++;
	TotalBotsSpawned = 0;
	CurrentNumOfBotsAlive = 0;
	MaxBotSpawnAmount = NumOfBotsToSpawn * CurrentWave;

	SetGameState(EMatchState::WaitingToStart);
	GetWorldTimerManager().SetTimer(StartMatchHandle, this, &ABattleTank_HordeMode::StartWave, TimeRemaining, false);
}

void ABattleTank_HordeMode::StartWave()
{
	GetWorldTimerManager().ClearTimer(StartMatchHandle);
	SetGameState(EMatchState::InProgress);
	CheckWaveProgress();
}

void ABattleTank_HordeMode::CheckWaveProgress()
{
	if (MaxBotSpawnAmount > 0 && TotalBotsSpawned < MaxBotSpawnAmount)
	{
		SpawnNewAIPawn();
	}
	else if (CurrentNumOfBotsAlive <= 0)
	{
		SetGameState(EMatchState::Completed);
		EndWave();
	}
}

void ABattleTank_HordeMode::EndWave()
{
	PrepareNewWave();
}


////////////////////////////////////////////////////////////////////////////////
// Game behaviour

void ABattleTank_HordeMode::OnAIBotDeath(AAIController * AICon)
{
	if (AICon)
	{
		CurrentNumOfBotsAlive--;
		CheckWaveProgress();
	}
}

void ABattleTank_HordeMode::OnPlayerDeath(APlayerController * PC)
{
	if (!PC) { return; }
	if (GameState && GameState->PlayerArray.Num() > 0)
	{
		for (int32 i = 0; i < GameState->PlayerArray.Num(); i++)
		{
			ATankPlayerState * PState = Cast<ATankPlayerState>(GameState->PlayerArray[i]);
			if (PState && !PState->GetIsPlayerDead()) { return; }
		}
	}

	// Only runs if all players are destroyed
	SetGameState(EMatchState::GameOver);
	GetWorldTimerManager().SetTimer(GameOverHandle, this, &ABattleTank_HordeMode::GameOver, 5, false);
}

void ABattleTank_HordeMode::SpawnNewAIPawn()
{
	GetWorldTimerManager().ClearTimer(SpawnPawnFailHandle);
	if (BotSpawnBoxArray.Num() > 0)
	{
		for (CurrentNumOfBotsAlive; CurrentNumOfBotsAlive < MaxBotAmountAtOnce; CurrentNumOfBotsAlive)
		{
			int32 RandNum = FMath::RandRange(0, BotSpawnBoxArray.Num() - 1);
			ASpawnBox_Pawn * SpawnBox = BotSpawnBoxArray[RandNum];
			if (SpawnBox && SpawnBox->PlacePawns(DefaultPawnAIClass, 500.f))
			{
				CurrentNumOfBotsAlive++;
				TotalBotsSpawned++;
				if (TotalBotsSpawned >= MaxBotSpawnAmount) { SetGameState(EMatchState::WaitingToComplete);  break; }
			}
			else
			{
				GetWorldTimerManager().SetTimer(SpawnPawnFailHandle, this, &ABattleTank_HordeMode::SpawnNewAIPawn, 5.f, false);
				break;
			}
		}
	}
}

