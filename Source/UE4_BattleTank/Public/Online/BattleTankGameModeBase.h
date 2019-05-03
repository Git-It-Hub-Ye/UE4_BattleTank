// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BattleTankGameModeBase.generated.h"

class AAIController;
class ABattleTankGameState;
class ASpawnBox_Actor;
class ASpawnBox_Pawn;
class ATank;
enum class EMatchState : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameTimeChanged);

UCLASS()
class UE4_BATTLETANK_API ABattleTankGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	/** Delegate called to update time on player hud */
	FOnGameTimeChanged OnTimeChanged;

protected:

	////////////////////////////////////////////////////////////////////////////////
	// Default classes

	/** The class for cameras on maps */
	UPROPERTY(EditDefaultsOnly, Category = "Classes")
	TSubclassOf<AActor> MapCameraClass;


	////////////////////////////////////////////////////////////////////////////////
	// Time

	/** Is game timed */
	UPROPERTY(EditDefaultsOnly, Category = "Config Time")
	bool bTimedMatch;

	/** Length of wait till match begins */
	UPROPERTY(EditDefaultsOnly, Category = "Config Time", meta = (ClampMin = 0, ClampMax = 60))
	int32 Time_WaitToStart;

	/** Length of match */
	UPROPERTY(EditDefaultsOnly, Category = "Config Time", meta = (ClampMin = 1, ClampMax = 1200))
	int32 Time_MatchLength;


	////////////////////////////////////////////////////////////////////////////////
	// Rounds

	/** Is game played over multiple rounds */
	UPROPERTY(EditDefaultsOnly, Category = "Config Rounds")
	bool bRoundBasedGame;

	/** length of wait between rounds */
	UPROPERTY(EditDefaultsOnly, Category = "Config Rounds", meta = (ClampMin = 1, ClampMax = 60))
	int32 Time_BetweenRounds;

	/** Max rounds that can be played */
	UPROPERTY(EditDefaultsOnly, Category = "Config Rounds", meta = (ClampMin = 0, ClampMax = 5))
	int32 MaxRounds;

	/** How many rounds must be won, for player to win the game */
	UPROPERTY(EditDefaultsOnly, Category = "Config Rounds", meta = (ClampMin = 0, ClampMax = 3))
	int32 RoundsToWin;

	/** Current round */
	int32 CurrentRound;


	////////////////////////////////////////////////////////////////////////////////
	// Score data

	/** Points for a kill */
	UPROPERTY(EditDefaultsOnly, Category = "Config Game", meta = (ClampMin = 1, ClampMax = 100))
	int32 KillPoints;

	/** Points for a kill */
	UPROPERTY(EditDefaultsOnly, Category = "Config Game", meta = (ClampMin = 1, ClampMax = 100))
	int32 AssistPoints;

	/** Min number of players required to play game */
	UPROPERTY(EditDefaultsOnly, Category = "Config Game", meta = (ClampMin = 1, ClampMax = 4))
	int32 MinPlayersRequired;

	/** Number of players in game */
	int32 NumOfPlayers;

	bool bIsGameInProgress;


	////////////////////////////////////////////////////////////////////////////////
	// Trigger data

	/** Triggers to spawn in world */
	UPROPERTY(EditDefaultsOnly, Category = "Config Trigger")
	TArray<TSubclassOf<AActor>> TriggerArray;

	/** Max number of triggers in world */
	UPROPERTY(EditDefaultsOnly, Category = "Config Trigger", meta = (ClampMin = 0, ClampMax = 10))
	int32 MaxTriggerNum;

	/** Current number of triggers in world */
	int32 CurrentTriggerNum;


	////////////////////////////////////////////////////////////////////////////////
	// Spawn boxes

	/** Array of spawn boxes to spawn ai within */
	TArray<ASpawnBox_Pawn*> BotSpawnBoxArray;

	/** Array of spawn boxes to spawn triggers within */
	TArray<ASpawnBox_Actor*> TriggerSpawnBoxArray;


	////////////////////////////////////////////////////////////////////////////////
	// Timer management

	/** Timer handle for match length */
	FTimerHandle TimerHandle_DefaultTimer;

	/** Timer handle for game over */
	FTimerHandle TimerHandle_GameOver;

	/** Timer handle for retrying spawn after failed attempts */
	FTimerHandle TimerHandle_SpawnTrigger;

private:
	/** How long till players outside combat area are killed */
	UPROPERTY(EditDefaultsOnly, Category = "Config Time", meta = (ClampMin = 0.f, ClampMax = 30.f))
	float TimeToExecution;

	/** Contains each player outside combat area and their timer to execution */
	TMap<ATank*, FTimerHandle> PlayerExecuteMap;

	/** Calls function with parameters by a timer */
	FTimerDelegate TimerDel;


public:
	ABattleTankGameModeBase();

	virtual void InitGame(const FString & MapName, const FString & Options, FString & ErrorMessage) override;

	virtual void PreInitializeComponents() override;

	virtual void PostLogin(APlayerController * NewPlayer) override;

	virtual void Logout(AController * Exiting) override;


	////////////////////////////////////////////////////////////////////////////////
	// Game behaviour

	/** Chooses player start location form player start actors in world */
	virtual AActor* ChoosePlayerStart_Implementation(AController * Player) override;

	/** Changes players camera target to world camera */
	void TransitionToMapCamera(APlayerController * PC);

	void PlayerOutsideCombatArea(ATank * PlayerPawn);

	void PlayerReturnedToCombatArea(ATank * PlayerPawn);


	////////////////////////////////////////////////////////////////////////////////
	// Events

	/** Call game behaviour and Change player state data */
	void HandleKill(AController * KilledPawn, AController * KillerPawn);

	/** Default behaviour when bot is killed */
	virtual void OnAIBotDeath(AAIController * AICon) PURE_VIRTUAL(ABattleTankGameModeBase::OnAIBotDeath,)

	/** Default behaviour when player is killed */
	virtual void OnPlayerDeath(APlayerController * PC) PURE_VIRTUAL(ABattleTankGameModeBase::OnPlayerDeath,)

	/** Default behaviour when bot is destroyed */
	void TriggerDestroyed();
	
protected:

	////////////////////////////////////////////////////////////////////////////////
	// Game behaviour

	/** Set game timer */
	void SetDefaultTimer(int32 NewTime);

	void RestartDfaultTimer();

	void PrepareToStartMatch();

	/** Starts game mode */
	virtual void StartMatch();

	/** Set Current game data */
	virtual void StartNewRound();

	/** Stops match and checks whether to end or continue match */
	void FinishMatch();

	/** Stop match behaviour */
	void StopMatch();

	/** Ends match */
	void EndGame();


	////////////////////////////////////////////////////////////////////////////////
	// Events

	/** Spawns ne trigger in world */
	void SpawnNewTrigger();


	////////////////////////////////////////////////////////////////////////////////
	// Notify game changes

	/** Notifies client that match has finished */
	void NotifyClientGameEnded();

	/** Updates current match display for player */
	void NotifyClientOfMatchUpdates();

	/** Updates match display score and player leaderboard */
	void UpdateMatchScoreDispaly();


	////////////////////////////////////////////////////////////////////////////////
	// Game data

	/** Set current state of game */
	void SetMatchState(EMatchState NewState);

	/** Gets all spawn boxes in world */
	void GetSpawnLocations();

	/** Find match winner */
	virtual void DetermineMatchWinner() PURE_VIRTUAL(ABattleTankGameModeBase::DetermineMatchWinner,)

private:

	void SetUpNewClient(APlayerController * NewPlayer);

	////////////////////////////////////////////////////////////////////////////////
	// Game behaviour

	/** Handle game countdown timer */
	void DefaultTimer();

	/** Updates player scoreboard */
	void SwitchToMapCamera();

	void SetExecutionTimer(ATank * PlayerPawn);

	void StopExecutionTimer(ATank * PlayerPawn);

	UFUNCTION()
	void ExecutePlayer(ATank * PlayerPawn);


	////////////////////////////////////////////////////////////////////////////////
	// Game data

	/** Sets up game data needed for client hud */
	void SetGameStateData();

	/** Get current game state */
	ABattleTankGameState * GetState() const;

};
