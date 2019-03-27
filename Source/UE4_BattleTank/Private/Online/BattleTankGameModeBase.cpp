// Copyright 2018 Stuart McDonald.

#include "BattleTankGameModeBase.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

#include "BattleTankGameState.h"
#include "TankPlayerState.h"
#include "GameFramework/PlayerController.h"
#include "Player/TankPlayerController.h"
#include "Player/Tank.h"
#include "UI/BattleHUD.h"
#include "Spawn/SpawnBox.h"
#include "Spawn/SpawnBox_Pawn.h"
#include "Spawn/SpawnBox_Actor.h"


ABattleTankGameModeBase::ABattleTankGameModeBase()
{
	GameStateClass = ABattleTankGameState::StaticClass();
	HUDClass = ABattleHUD::StaticClass();
	PlayerStateClass = ATankPlayerState::StaticClass();
	DefaultPawnClass = ATank::StaticClass();

	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Dynamic/Tank/Behaviour/BP_Tank_Player"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	bHasTimer = true;
	bHasRounds = false;

	Time_WaitToStart = 10;
	Time_MatchLength = 600;
	Time_BetweenRounds = 10;

	KillPoints = 100;
	AssistPoints = 25;

	MinPlayersRequired = 1;
	MaxTriggerNum = 4;
	CurrentTriggerNum = 0;
	CurrentRound = 0;
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
	SetMatchState(EMatchState::WaitingForPlayers);
	SetGameStateData();
}

void ABattleTankGameModeBase::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);

	// TODO notify player when joined if match already in progress

	ATankPlayerController * PC = Cast<ATankPlayerController>(NewPlayer);
	if (PC)
	{
		PC->ClientInGame();
	}

	NumOfPlayers++;
	if (NumOfPlayers >= MinPlayersRequired)
	{
		Time_WaitToStart > 0 ? PrepareToStartMatch() : StartMatch();
	}
	else
	{
		SetMatchState(EMatchState::WaitingForPlayers);
		NotifyClientOfMatchState();
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

void ABattleTankGameModeBase::DefaultTimer()
{
	ABattleTankGameState * GS = Cast<ABattleTankGameState>(GameState);
	if (GS)
	{
		GS->RemainingTime--;
		OnTimeChanged.Broadcast();

		if (GS->RemainingTime <= 0)
		{
			GetWorldTimerManager().ClearTimer(TimerHandle_DefaultTimer);

			if (GS->GetMatchState() == EMatchState::WaitingToStart)
			{
				StartMatch();
			}
			else if (GS->GetMatchState() == EMatchState::InProgress)
			{
				FinishMatch();
			}
		}
	}
}

void ABattleTankGameModeBase::SetDefaultTimer(int32 NewTime)
{
	GetWorldTimerManager().ClearTimer(TimerHandle_DefaultTimer);

	ABattleTankGameState * GS = Cast<ABattleTankGameState>(GameState);
	if (GS)
	{
		GS->RemainingTime = NewTime;
		OnTimeChanged.Broadcast();
	}
}

void ABattleTankGameModeBase::RestartDfaultTimer()
{
	GetWorldTimerManager().SetTimer(TimerHandle_DefaultTimer, this, &ABattleTankGameModeBase::DefaultTimer, GetWorldSettings()->GetEffectiveTimeDilation(), true);
}

void ABattleTankGameModeBase::PrepareToStartMatch()
{
	SetMatchState(EMatchState::WaitingToStart);
	NotifyClientOfMatchState();
	SetDefaultTimer(Time_WaitToStart);
	RestartDfaultTimer();
}

void ABattleTankGameModeBase::StartMatch()
{
	SetMatchState(EMatchState::InProgress);
	NotifyClientOfMatchState();
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ATankPlayerController * PC = Cast<ATankPlayerController>(*It);
		if (PC)
		{
			PC->ClientMatchStarted();
		}
	}
}

void ABattleTankGameModeBase::StartNewRound()
{
	CurrentRound++;

	ABattleTankGameState * GS = GetWorld()->GetGameState<ABattleTankGameState>();
	if (GS)
	{
		GS->CurrentRound = CurrentRound;
	}
	UpdateMatchScoreboard();
}

void ABattleTankGameModeBase::FinishMatch()
{
	SetMatchState(EMatchState::Finished);
	StopMatch();
	NotifyClientOfMatchState();

	GetWorldTimerManager().SetTimer(TimerHandle_GameOver, this, &ABattleTankGameModeBase::EndGame, 5, false);
}

void ABattleTankGameModeBase::StopMatch()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_DefaultTimer);
	GetWorldTimerManager().ClearTimer(TimerHandle_SpawnTriggerFail);
}

void ABattleTankGameModeBase::EndGame()
{
	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		(*It)->TurnOff();
	}

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ATankPlayerController * PC = Cast<ATankPlayerController>(*It);
		if (PC)
		{
			PC->ClientGameEnded();
			TransitionToMapCamera(PC);
		}
	}
	GetWorldTimerManager().SetTimer(TimerHandle_GameOver, this, &ABattleTankGameModeBase::EndMatchScoreboard, 5, false);
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
			BHUD->ShowLeaderboard(true);
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
// Events

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

	if (KillerPlayerState || VictimPlayerState) { UpdateMatchScoreboard(); }
}

void ABattleTankGameModeBase::TriggerDestroyed()
{
	CurrentTriggerNum--;
	SpawnNewTrigger();
}

void ABattleTankGameModeBase::SpawnNewTrigger()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_SpawnTriggerFail);

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
				GetWorldTimerManager().SetTimer(TimerHandle_SpawnTriggerFail, this, &ABattleTankGameModeBase::SpawnNewTrigger, 5.f, false);
				break;
			}
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
// Notify about Game changes

void ABattleTankGameModeBase::NotifyClientGameEnded()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ATankPlayerController * PC = Cast<ATankPlayerController>(*It);
		if (PC)
		{
			PC->ClientGameEnded();
		}
	}
}

void ABattleTankGameModeBase::NotifyClientOfMatchState()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ATankPlayerController * PC = Cast<ATankPlayerController>(*It);
		if (PC)
		{
			PC->ClientNotifyOfMatchState();
		}
	}
}

void ABattleTankGameModeBase::UpdateMatchScoreboard()
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


////////////////////////////////////////////////////////////////////////////////
// Game data

void ABattleTankGameModeBase::SetGameStateData()
{
	ABattleTankGameState * GS = Cast<ABattleTankGameState>(GameState);
	if (GS)
	{
		GS->SetGameDataUsed(bHasTimer, bHasRounds);
	}
}

void ABattleTankGameModeBase::SetMatchState(EMatchState NewState)
{
	ABattleTankGameState * GS = GetGameState<ABattleTankGameState>();
	if (GS)
	{
		GS->SetMatchState(NewState);
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
