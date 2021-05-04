// Copyright 2018 to 2021 Stuart McDonald.

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

	bTimedMatch = true;
	bRoundBasedGame = false;
	bIsGameInProgress = false;

	TimeToExecution = 10.f;
	Time_WaitToStart = 10;
	Time_MatchLength = 600;
	Time_BetweenRounds = 10;

	KillPoints = 100;
	AssistPoints = 25;

	NumOfPlayers = 0;
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

	SetUpNewClient(NewPlayer);

	NumOfPlayers++;
	if (!bIsGameInProgress && NumOfPlayers >= MinPlayersRequired)
	{
		bIsGameInProgress = true;
		Time_WaitToStart > 0 ? PrepareToStartMatch() : StartMatch();
	}
	else if (NumOfPlayers < MinPlayersRequired)
	{
		SetMatchState(EMatchState::WaitingForPlayers);
		NotifyClientOfMatchUpdates();
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
	NotifyClientOfMatchUpdates();
	SetDefaultTimer(Time_WaitToStart);
	RestartDfaultTimer();
}

void ABattleTankGameModeBase::StartMatch()
{
	SetMatchState(EMatchState::InProgress);
	NotifyClientOfMatchUpdates();
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ATankPlayerController * PC = Cast<ATankPlayerController>(*It);
		if (PC)
		{
			PC->ClientMatchStarted();
		}
	}

	if (!bRoundBasedGame && bTimedMatch)
	{
		SetDefaultTimer(Time_MatchLength);
		RestartDfaultTimer();
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

	NotifyClientOfMatchUpdates();

	if (bTimedMatch)
	{
		SetDefaultTimer(Time_MatchLength);
		RestartDfaultTimer();
	}
}

void ABattleTankGameModeBase::FinishMatch()
{
	// Stop all pawn movement and disable input for whatever controller was assigned to it
	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		(*It)->DisableInput(nullptr);
		(*It)->TurnOff();
	}

	SetMatchState(EMatchState::Finished);
	StopMatch();
	NotifyClientOfMatchUpdates();
	EndGame();
}

void ABattleTankGameModeBase::StopMatch()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_DefaultTimer);
	GetWorldTimerManager().ClearTimer(TimerHandle_SpawnTrigger);
}

void ABattleTankGameModeBase::EndGame()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ATankPlayerController * PC = Cast<ATankPlayerController>(*It);
		if (PC)
		{
			ABattleHUD * BHUD = PC ? Cast<ABattleHUD>(PC->GetHUD()) : nullptr;
			if (BHUD)
			{
				PC->ClientMatchFinished();
				BHUD->ShowLeaderboard(true);
			}
		}
	}
	GetWorldTimerManager().SetTimer(TimerHandle_SwitchToMapCam, this, &ABattleTankGameModeBase::SwitchToMapCamera, 3, false);
	GetWorldTimerManager().SetTimer(TimerHandle_ReturnPlayers, this, &ABattleTankGameModeBase::ReturnPlayersToMainMenu, 10, false);
}

void ABattleTankGameModeBase::ReturnPlayersToMainMenu()
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		ATankPlayerController * LocalPrimaryController = Cast<ATankPlayerController>(*Iterator);
		if (LocalPrimaryController && LocalPrimaryController->IsLocalController())
		{
			LocalPrimaryController->HandleReturnToMainMenu();
		}
	}
}

void ABattleTankGameModeBase::SwitchToMapCamera()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ATankPlayerController * PC = Cast<ATankPlayerController>(*It);
		if (PC)
		{
			PC->ClientGameEnded();
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

void ABattleTankGameModeBase::PlayerOutsideCombatArea(ATank * PlayerPawn)
{
	SetExecutionTimer(PlayerPawn);
}

void ABattleTankGameModeBase::PlayerReturnedToCombatArea(ATank * PlayerPawn)
{
	StopExecutionTimer(PlayerPawn);
}

void ABattleTankGameModeBase::SetExecutionTimer(ATank * PlayerPawn)
{
	if (PlayerPawn)
	{
		FTimerHandle Handle_DeathTimer;

		TimerDel.BindUFunction(this, FName("ExecutePlayer"), PlayerPawn);

		GetWorldTimerManager().SetTimer(Handle_DeathTimer, TimerDel, TimeToExecution, false);

		PlayerExecuteMap.Add(PlayerPawn, Handle_DeathTimer);
	}
}

void ABattleTankGameModeBase::StopExecutionTimer(ATank * PlayerPawn)
{
	if (PlayerExecuteMap.Contains(PlayerPawn))
	{
		FTimerHandle Handle_MapTimer = *PlayerExecuteMap.Find(PlayerPawn);
		if (Handle_MapTimer.IsValid())
		{
			GetWorldTimerManager().ClearTimer(Handle_MapTimer);
		}
		PlayerExecuteMap.Remove(PlayerPawn);
	}
}

void ABattleTankGameModeBase::ExecutePlayer(ATank * PlayerPawn)
{
	if (PlayerExecuteMap.Contains(PlayerPawn))
	{
		FTimerHandle Handle_DeathTimer = *PlayerExecuteMap.Find(PlayerPawn);
		if (Handle_DeathTimer.IsValid())
		{
			GetWorldTimerManager().ClearTimer(Handle_DeathTimer);
		}

		if (PlayerPawn && PlayerPawn->IsPlayerControlled() && !PlayerPawn->IsTankDestroyed())
		{
			PlayerPawn->Execute();
		}
		PlayerExecuteMap.Remove(PlayerPawn);
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

	if (KillerPlayerState || VictimPlayerState) { UpdateMatchScoreDispaly(); }
}

void ABattleTankGameModeBase::TriggerDestroyed()
{
	CurrentTriggerNum--;
	GetWorldTimerManager().SetTimer(TimerHandle_SpawnTrigger, this, &ABattleTankGameModeBase::SpawnNewTrigger, 3, false);
}

void ABattleTankGameModeBase::SpawnNewTrigger()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_SpawnTrigger);

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
				GetWorldTimerManager().SetTimer(TimerHandle_SpawnTrigger, this, &ABattleTankGameModeBase::SpawnNewTrigger, 5.f, false);
				break;
			}
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
// Notify client

void ABattleTankGameModeBase::SetUpNewClient(APlayerController * NewPlayer)
{
	ATankPlayerController * PC = Cast<ATankPlayerController>(NewPlayer);
	if (PC)
	{
		PC->ClientInGame();
		if (bIsGameInProgress)
		{
			PC->ClientMatchStarted();
		}
	}
}

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

void ABattleTankGameModeBase::NotifyClientOfMatchUpdates()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ATankPlayerController * PC = Cast<ATankPlayerController>(*It);
		ABattleHUD * BHUD = PC ? Cast<ABattleHUD>(PC->GetHUD()) : nullptr;
		if (BHUD)
		{
			BHUD->UpdateMatchStateDisplay();
		}
	}
}

void ABattleTankGameModeBase::UpdateMatchScoreDispaly()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController * PC = Cast<APlayerController>(*It);
		ABattleHUD * BHUD = PC ? Cast<ABattleHUD>(PC->GetHUD()) : nullptr;
		if (BHUD)
		{
			BHUD->UpdateMatchScores();
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
		GS->SetGameDataUsed(bTimedMatch, bRoundBasedGame);
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
