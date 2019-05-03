// Copyright 2018 Stuart McDonald.

#include "BattleTank_HordeMode.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"
#include "TimerManager.h"

#include "BattleTankGameState.h"
#include "GameFramework/PlayerController.h"
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

	bGameOver = false;

	bTimedMatch = false;
	bRoundBasedGame = true;

	Time_DelayBotSpawn = 5;
	Time_WaitToStart = 10;
	Time_BetweenRounds = 3;

	NumOfBotsPerPlayer = 4;
	MaxBotAmountAtOnce = 10;
	MaxBotNumThisWave = 4;
	TotalBotsSpawned = 0;
	CurrentNumOfBotsAlive = 0;
}


////////////////////////////////////////////////////////////////////////////////
// Game behaviour

void ABattleTank_HordeMode::StartMatch()
{
	Super::StartMatch();
	PrepareForNextWave();
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
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController * PCon = Cast<APlayerController>(*It);
		if (!PCon || !PCon->GetPawn()) { return; }

		ATank * PCTank = Cast<ATank>(PCon->GetPawn());
		if (PCTank && !PCTank->IsTankDestroyed()) { return; }
	}

	// Only runs if all players are destroyed
	GetWorldTimerManager().ClearTimer(TimerHandle_SpawnPawnFail);
	bGameOver = true;
	FinishMatch();
}


////////////////////////////////////////////////////////////////////////////////
// Handle Waves

void ABattleTank_HordeMode::PrepareForNextWave()
{
	TotalBotsSpawned = 0;
	CurrentNumOfBotsAlive = 0;
	GetWorldTimerManager().SetTimer(TimerHandle_StartWave, this, &ABattleTank_HordeMode::StartWave, Time_BetweenRounds, false);
}

void ABattleTank_HordeMode::StartWave()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_StartWave);
	StartNewRound();
	SetNumOfBotsToSpawn();

	GetWorldTimerManager().SetTimer(TimerHandle_StartWave, this, &ABattleTank_HordeMode::CheckWaveProgress, Time_DelayBotSpawn, false);
}

void ABattleTank_HordeMode::SetNumOfBotsToSpawn()
{
	if (CurrentRound < 5)
	{
		MaxBotNumThisWave = (CurrentRound * 2) + (NumOfPlayers * NumOfBotsPerPlayer);
	}
	else
	{
		MaxBotNumThisWave += (NumOfPlayers * NumOfBotsPerPlayer);
	}
}

void ABattleTank_HordeMode::CheckWaveProgress()
{
	if (MaxBotNumThisWave > 0 && TotalBotsSpawned < 1)
	{
		SpawnNewAIPawn();
	}
	else if (CurrentNumOfBotsAlive <= 0)
	{
		PrepareForNextWave();
	}
}

void ABattleTank_HordeMode::SpawnNewAIPawn()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_SpawnPawnFail);
	if (BotSpawnBoxArray.Num() > 0)
	{
		for (CurrentNumOfBotsAlive; CurrentNumOfBotsAlive < 1; CurrentNumOfBotsAlive)
		{
			if (bGameOver) { return; }

			int32 RandNum = FMath::RandRange(0, BotSpawnBoxArray.Num() - 1);
			ASpawnBox_Pawn * SpawnBox = BotSpawnBoxArray[RandNum];
			if (SpawnBox && SpawnBox->PlacePawns(DefaultPawnAIClass, 500.f))
			{
				CurrentNumOfBotsAlive++;
				TotalBotsSpawned++;
				if (TotalBotsSpawned >= MaxBotNumThisWave) { break; }
			}
			else
			{
				GetWorldTimerManager().SetTimer(TimerHandle_SpawnPawnFail, this, &ABattleTank_HordeMode::SpawnNewAIPawn, 5.f, false);
				break;
			}
		}
	}
}

