// Copyright 2018 Stuart McDonald.

#include "BattleTank_HordeMode.h"
#include "UObject/ConstructorHelpers.h"
#include "BattleTankGameState.h"
#include "TankPlayerState.h"
#include "TimerManager.h"
#include "SpawnBox_Pawn.h"

ABattleTank_HordeMode::ABattleTank_HordeMode()
{
	bHasTimer = false;
	bMultipleRounds = true;
	bAllowBots = true;

	NumOfBotsToSpawn = 10;
	MaxBotAmountAtOnce = 10;
}


////////////////////////////////////////////////////////////////////////////////
// Handle Waves

void ABattleTank_HordeMode::PrepareNewWave()
{
	GetWorldTimerManager().ClearTimer(StartMatchHandle);

	CurrentRound++;
	TotalBotsSpawned = 0;
	CurrentNumOfBotsAlive = 0;
	MaxBotSpawnAmount = NumOfBotsToSpawn * CurrentRound;

	NewRound();

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
	GetWorldTimerManager().SetTimer(StartMatchHandle, this, &ABattleTank_HordeMode::PrepareNewWave, 3.f, false);
}


////////////////////////////////////////////////////////////////////////////////
// Game behaviour

void ABattleTank_HordeMode::StartGame()
{
	Super::StartGame();
	GetWorldTimerManager().SetTimer(StartMatchHandle, this, &ABattleTank_HordeMode::PrepareNewWave, 5.f, false);
}

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
	GetWorldTimerManager().SetTimer(GameOverHandle, this, &ABattleTank_HordeMode::FinishMatch, 5, false);
}

