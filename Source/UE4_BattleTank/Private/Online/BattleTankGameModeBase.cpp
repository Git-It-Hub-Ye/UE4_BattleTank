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
#include "TankPlayerState.h"
#include "UI/BattleHUD.h"
#include "Player/Tank.h"
#include "GameFramework/GameState.h"
#include "Kismet/GameplayStatics.h"
#include "Online/BattleTankGameState.h"


ABattleTankGameModeBase::ABattleTankGameModeBase()
{
	GameStateClass = ABattleTankGameState::StaticClass();
	HUDClass = ABattleHUD::StaticClass();
	PlayerStateClass = ATankPlayerState::StaticClass();
	DefaultPawnAIClass = ATank::StaticClass();
	DefaultPawnClass = ATank::StaticClass();

	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Dynamic/Tank/Behaviour/BP_Tank_Player"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	static ConstructorHelpers::FClassFinder<APawn> AIBotPawnBPClass(TEXT("/Game/Dynamic/Tank/Behaviour/BP_Tank_AI"));
	if (AIBotPawnBPClass.Class != NULL)
	{
		DefaultPawnAIClass = AIBotPawnBPClass.Class;
	}

	TimeBetweenWaves = 10;

	NumOfBotsToSpawn = 10;
	MaxBotsAtOnce = 10;

	MaxBotsThisRound = 0;
	TotalBotsSpawned = 0;
	CurrentNumOfBotsAlive = 0;
	CurrentWave = 0;
	KillPoints = 100;
}

void ABattleTankGameModeBase::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void ABattleTankGameModeBase::Logout(AController * Exiting)
{
}

AActor * ABattleTankGameModeBase::ChoosePlayerStart_Implementation(AController * Player)
{
	return Super::ChoosePlayerStart_Implementation(Player);
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

	SetGameState(EMatchState::WaitingToStart);
	GetWorldTimerManager().SetTimer(NextWaveHandle, this, &ABattleTankGameModeBase::StartWave, TimeBetweenWaves, false);
}

void ABattleTankGameModeBase::StartWave()
{
	GetWorldTimerManager().ClearTimer(NextWaveHandle);
	SetGameState(EMatchState::WaveInProgress);
	ContinueMatch();
}

void ABattleTankGameModeBase::ContinueMatch()
{
	if (MaxBotsThisRound > 0 && TotalBotsSpawned < MaxBotsThisRound)
	{
		SpawnNewAIPawn();
	}
	else if (CurrentNumOfBotsAlive <= 0)
	{
		SetGameState(EMatchState::WaveCompleted);
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
			if (SpawnBox && SpawnBox->PlacePawns(DefaultPawnAIClass, 500.f))
			{
				CurrentNumOfBotsAlive++;
				TotalBotsSpawned++;
				if (TotalBotsSpawned >= MaxBotsThisRound) { SetGameState(EMatchState::WaitingToComplete);  break; }
			}
			else
			{
				FailedSpawns++;
				if (FailedSpawns >= 10) { break; }
			}
		}
	}
}

void ABattleTankGameModeBase::SpawnNewTrigger()
{
	if (ActorSpawnBoxArray.Num() > 0)
	{
		int32 RandNum = FMath::RandRange(0, ActorSpawnBoxArray.Num() - 1);
		ASpawnBox_Actor * SpawnBox = ActorSpawnBoxArray[RandNum];
		if (SpawnBox)
		{
			SpawnBox->PlaceActors();
		}
	}
}

void ABattleTankGameModeBase::AIBotDestroyed(AController * AICon)
{
	if (AICon)
	{
		CurrentNumOfBotsAlive--;
		ContinueMatch();
	}
}

void ABattleTankGameModeBase::PlayerDestroyed()
{
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
	GetWorldTimerManager().SetTimer(GameOverHandle, this, &ABattleTankGameModeBase::GameOver, 5, false);
}

void ABattleTankGameModeBase::TriggerDestroyed()
{
	SpawnNewTrigger();
}

void ABattleTankGameModeBase::HandleKill(AController * KilledPawn, AController * KillerPawn)
{
	ATankPlayerState * KillerPlayerState = KillerPawn ? Cast<ATankPlayerState>(KillerPawn->PlayerState) : NULL;
	ATankPlayerState * VictimPlayerState = KilledPawn ? Cast<ATankPlayerState>(KilledPawn->PlayerState) : NULL;

	if (KillerPlayerState && KillerPlayerState != VictimPlayerState)
	{
		KillerPlayerState->ScoreKill(KillPoints);
	}

	if (VictimPlayerState)
	{
		VictimPlayerState->ScoreDeath();
	}

	if (KillerPlayerState || VictimPlayerState) { UpdatePlayerScoreboard(); }
}

void ABattleTankGameModeBase::SetGameState(EMatchState NewState)
{
	ABattleTankGameState * GS = GetGameState<ABattleTankGameState>();

	if (GS)
	{
		GS->SetMatchState(NewState);
	}
}

void ABattleTankGameModeBase::UpdatePlayerScoreboard()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController * PC = Cast<APlayerController>(*It);
		ABattleHUD * BHUD = PC ? Cast<ABattleHUD>(PC->GetHUD()) : nullptr;
		if (BHUD)
		{
			BHUD->UpdateLeaderboard();
		}
	}
}

void ABattleTankGameModeBase::UpdateMatchStateScoreboard()
{
	UE_LOG(LogTemp, Warning, TEXT("Point"))
}

void ABattleTankGameModeBase::ShowEndMatchScoreboard()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController * PC = Cast<APlayerController>(*It);
		ABattleHUD * BHUD = PC ? Cast<ABattleHUD>(PC->GetHUD()) : nullptr;
		if (BHUD)
		{
			PC->DisableInput(PC);
			BHUD->RemoveWidgetsOnGameOver();
			BHUD->UpdateMatchEndDisplay();
		}
	}
}

void ABattleTankGameModeBase::GameOver()
{
	ShowEndMatchScoreboard();
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController * PC = Cast<APlayerController>(*It);
		if (PC)
		{
			TransitionToMapCamera(PC);
		}
	}
}

void ABattleTankGameModeBase::TransitionToMapCamera(APlayerController * PC)
{
	if (!PC) { return; }

	TArray<AActor*> ReturnedCameras;
	UGameplayStatics::GetAllActorsOfClass(this, MapCameraClass, ReturnedCameras);

	if (ReturnedCameras.Num() > 0)
	{
		AActor * NewMapCamera = ReturnedCameras[0];
		PC->SetViewTarget(NewMapCamera);
	}
}

