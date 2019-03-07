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
#include "Player/TankPlayerController.h"
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
	DefaultPawnClass = ATank::StaticClass();
	DefaultPawnAIClass = ATank::StaticClass();

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

	TimeRemaining = 10;
	KillPoints = 100;
	AssistPoints = 25;

	MinPlayersRequired = 1;
	MaxTriggerNum = 4;
	CurrentTriggerNum = 0;
	CurrentRound = 0;

	bMultipleRounds = false;
	bHasTimer = true;

	NumOfBotsToSpawn = 0;
	MaxBotAmountAtOnce = 0;
	MaxBotSpawnAmount = 0;
	TotalBotsSpawned = 0;
	CurrentNumOfBotsAlive = 0;

	bAllowBots = false;
	bInfiniteBots = false;
}

void ABattleTankGameModeBase::InitGame(const FString & MapName, const FString & Options, FString & ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	GetSpawnLocations();
	SpawnNewTrigger();
}

void ABattleTankGameModeBase::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	SetGameStateData();
}

void ABattleTankGameModeBase::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if (NewPlayer)
	{
		NumOfPlayers++;
		NotifyClientGameCreated(NewPlayer);

		if (NumOfPlayers >= MinPlayersRequired)
		{
			StartGame();
		}
	}
}

void ABattleTankGameModeBase::Logout(AController * Exiting)
{
}


////////////////////////////////////////////////////////////////////////////////
// Game behaviour

AActor * ABattleTankGameModeBase::ChoosePlayerStart_Implementation(AController * Player)
{
	return Super::ChoosePlayerStart_Implementation(Player);
}

void ABattleTankGameModeBase::StartGame()
{
	// Empty on default
}

void ABattleTankGameModeBase::FinishMatch()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController * PC = Cast<APlayerController>(*It);
		if (PC)
		{
			TransitionToMapCamera(PC);
		}
	}
	GetWorldTimerManager().SetTimer(GameOverHandle, this, &ABattleTankGameModeBase::EndMatchScoreboard, TimeRemaining, false);
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

void ABattleTankGameModeBase::EndMatchScoreboard()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController * PC = Cast<APlayerController>(*It);
		ABattleHUD * BHUD = PC ? Cast<ABattleHUD>(PC->GetHUD()) : nullptr;
		if (BHUD)
		{
			PC->DisableInput(PC);
			BHUD->RemoveWidgetsOnGameOver();
			BHUD->ShowLeaderboard(true);
		}
	}
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

	if (KillerPlayerState || VictimPlayerState) { UpdateScoreboard(); }
}

void ABattleTankGameModeBase::OnAIBotDeath(AAIController * AICon)
{
	// Empty on default
}

void ABattleTankGameModeBase::OnPlayerDeath(APlayerController * PC)
{
	 // Empty on default
}

void ABattleTankGameModeBase::TriggerDestroyed()
{
	CurrentTriggerNum--;
	SpawnNewTrigger();
}

void ABattleTankGameModeBase::SpawnNewTrigger()
{
	GetWorldTimerManager().ClearTimer(SpawnTriggerFailHandle);

	if (TriggerArray.Num() > 0 && TriggerSpawnBoxArray.Num() > 0)
	{
		for (CurrentTriggerNum; CurrentTriggerNum < MaxTriggerNum; CurrentTriggerNum)
		{
			int32 RandBox = FMath::RandRange(0, TriggerSpawnBoxArray.Num() - 1);
			int32 RandTrigger = FMath::RandRange(0, TriggerArray.Num() - 1);
			ASpawnBox_Actor * SpawnBox = TriggerSpawnBoxArray[RandBox];
			
			if (SpawnBox && SpawnBox->PlaceActors(TriggerArray[RandTrigger], 500.f))
			{
				CurrentTriggerNum++;
			}
			else
			{
				GetWorldTimerManager().SetTimer(SpawnTriggerFailHandle, this, &ABattleTankGameModeBase::SpawnNewTrigger, 5.f, false);
				break;
			}
		}
	}
}

void ABattleTankGameModeBase::SpawnNewAIPawn()
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
				GetWorldTimerManager().SetTimer(SpawnPawnFailHandle, this, &ABattleTankGameModeBase::SpawnNewAIPawn, 5.f, false);
				break;
			}
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
// Game data

void ABattleTankGameModeBase::SetGameStateData()
{
	ABattleTankGameState * GS = GetWorld()->GetGameState<ABattleTankGameState>();
	if (GS)
	{
		GS->SetGameDataUsed(bHasTimer, bMultipleRounds);
	}
}

void ABattleTankGameModeBase::NewRound()
{
	ABattleTankGameState * GS = GetWorld()->GetGameState<ABattleTankGameState>();
	if (GS)
	{
		GS->UpdateCurrentRound();
		NotifyClientMatchStart();
	}
}

void ABattleTankGameModeBase::GetSpawnLocations()
{
	for (TActorIterator<ASpawnBox> i(GetWorld()); i; ++i)
	{
		ASpawnBox_Actor * ActorSpawnBox = Cast<ASpawnBox_Actor>(*i);
		if (ActorSpawnBox)
		{
			TriggerSpawnBoxArray.Add(ActorSpawnBox);
		}

		if (!bAllowBots) { continue; }
		ASpawnBox_Pawn * BotSpawnBox = Cast<ASpawnBox_Pawn>(*i);
		if (BotSpawnBox)
		{
			BotSpawnBoxArray.Add(BotSpawnBox);
		}
	}
}

ABattleTankGameState * ABattleTankGameModeBase::GetState() const
{
	return GetWorld()->GetGameState<ABattleTankGameState>();
}


////////////////////////////////////////////////////////////////////////////////
// Notify about Game changes

void ABattleTankGameModeBase::SetGameState(EMatchState NewState)
{
	ABattleTankGameState * GS = GetGameState<ABattleTankGameState>();
	if (GS)
	{
		GS->SetMatchState(NewState);
	}
}

void ABattleTankGameModeBase::NotifyClientGameCreated(APlayerController * NewPlayer)
{
	ATankPlayerController * PC = Cast<ATankPlayerController>(NewPlayer);
	if (PC)
	{
		PC->ClientInGame();
	}
}

void ABattleTankGameModeBase::NotifyClientMatchStart()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ATankPlayerController * PC = Cast<ATankPlayerController>(*It);
		if (PC)
		{
			PC->ClientGameStarted();
		}
	}
}

void ABattleTankGameModeBase::UpdateScoreboard()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController * PC = Cast<APlayerController>(*It);
		ABattleHUD * BHUD = PC ? Cast<ABattleHUD>(PC->GetHUD()) : nullptr;
		if (BHUD)
		{
			BHUD->UpdateScoreboard();
			BHUD->UpdateLeaderboard();
		}
	}
}

