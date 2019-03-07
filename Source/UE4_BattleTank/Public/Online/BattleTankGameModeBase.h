// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BattleTankGameModeBase.generated.h"

class AAIController;
class ABattleTankGameState;
class ASpawnBox_Actor;
class ASpawnBox_Pawn;
enum class EMatchState : uint8;

UCLASS()
class UE4_BATTLETANK_API ABattleTankGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:

	////////////////////////////////////////////////////////////////////////////////
	// Default classes

	/** The class for cameras on maps */
	UPROPERTY(EditDefaultsOnly, Category = "Classes")
	TSubclassOf<AActor> MapCameraClass;

	/** The ai pawn class */
	UPROPERTY(EditDefaultsOnly, Category = "Classes")
	TSubclassOf<APawn> DefaultPawnAIClass;

	/** Triggers to spawn in world */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TArray<TSubclassOf<AActor>> TriggerArray;


	////////////////////////////////////////////////////////////////////////////////
	// Game mode data

	/** Time till NextWave */
	UPROPERTY(EditDefaultsOnly, Category = "Config", meta = (ClampMin = 1, ClampMax = 60))
	int32 TimeRemaining;

	/** Points for a kill */
	UPROPERTY(EditDefaultsOnly, Category = "Config", meta = (ClampMin = 1, ClampMax = 100))
	int32 KillPoints;

	/** Points for a kill */
	UPROPERTY(EditDefaultsOnly, Category = "Config", meta = (ClampMin = 1, ClampMax = 100))
	int32 AssistPoints;

	/** Min number of players required to play game */
	UPROPERTY(EditDefaultsOnly, Category = "Config", meta = (ClampMin = 1, ClampMax = 4))
	int32 MinPlayersRequired;

	/** Number of players in game */
	int32 NumOfPlayers;

	/** Max number of triggers in world */
	UPROPERTY(EditDefaultsOnly, Category = "Config", meta = (ClampMin = 0, ClampMax = 10))
	int32 MaxTriggerNum;

	/** Current number of triggers in world */
	int32 CurrentTriggerNum;

	/** Current match round */
	int32 CurrentRound;

	/** Does this game mode have a timer */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	bool bHasTimer;

	/** Does this game mode have rounds */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	bool bMultipleRounds;


	////////////////////////////////////////////////////////////////////////////////
	// Game mode bot data

	/** Number of AI bots to spawn in first round. For the following rounds this will be multiplied by the current round */
	UPROPERTY(EditDefaultsOnly, Category = "Config", meta = (ClampMin = 1, ClampMax = 20))
	int32 NumOfBotsToSpawn;

	/** Max AI bots alive in world at same time */
	UPROPERTY(EditDefaultsOnly, Category = "Config", meta = (ClampMin = 1, ClampMax = 20))
	int32 MaxBotAmountAtOnce;

	/** Keeps track of total AI bots spawned */
	int32 TotalBotsSpawned;

	/** Number of bots currently active */
	int32 CurrentNumOfBotsAlive;

	/** Max number of bots this wave */
	int32 MaxBotSpawnAmount;

	/** Does this game mode spawn bots */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	bool bAllowBots;

	/** Does this game mode spawn an unlimited number of bots */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	bool bInfiniteBots;


	////////////////////////////////////////////////////////////////////////////////
	// Spawn boxes

	/** Array of spawn boxes to spawn ai within */
	TArray<ASpawnBox_Pawn*> BotSpawnBoxArray;

	/** Array of spawn boxes to spawn triggers within */
	TArray<ASpawnBox_Actor*> TriggerSpawnBoxArray;


	////////////////////////////////////////////////////////////////////////////////
	// Timer management

	/** Timer handle for game start */
	FTimerHandle StartMatchHandle;

	/** Timer handle for game over */
	FTimerHandle GameOverHandle;

	/** Timer handle for retrying spawn after failed attempts */
	FTimerHandle SpawnPawnFailHandle;

	/** Timer handle for retrying spawn after failed attempts */
	FTimerHandle SpawnTriggerFailHandle;

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

	/** Call game behaviour and Change player state data */
	void HandleKill(AController * KilledPawn, AController * KillerPawn);

	/** Default behaviour when bot is killed */
	virtual void OnAIBotDeath(AAIController * AICon);

	/** Default behaviour when player is killed */
	virtual void OnPlayerDeath(APlayerController * PC);

	/** Default behaviour when bot is destroyed */
	void TriggerDestroyed();

	/** Changes players camera target to world camera */
	void TransitionToMapCamera(APlayerController * PC);
	
protected:

	////////////////////////////////////////////////////////////////////////////////
	// Game behaviour

	/** Starts game mode */
	virtual void StartGame();

	/** Ends match */
	void FinishMatch();

	/** Spawns ne trigger in world */
	void SpawnNewTrigger();

	/** Spawns ai at empty location */
	void SpawnNewAIPawn();


	////////////////////////////////////////////////////////////////////////////////
	// Notify about Game changes

	/** Set current state of game */
	void SetGameState(EMatchState NewState);

	/** Notifies client that a game has been created */
	void NotifyClientGameCreated(APlayerController * NewPlayer);

	/** Notifies client that match has started */
	void NotifyClientMatchStart();

	/** Updates player leaderboard */
	void UpdateScoreboard();


	////////////////////////////////////////////////////////////////////////////////
	// Game data

	/** Set Current game data */
	void NewRound();

	/** Gets all spawn boxes in world */
	void GetSpawnLocations();

private:

	////////////////////////////////////////////////////////////////////////////////
	// Game behaviour

	/** Updates player scoreboard */
	void EndMatchScoreboard();


	////////////////////////////////////////////////////////////////////////////////
	// Game data

	/** Sets up game data needed for client hud */
	void SetGameStateData();

	/** Get current game state */
	ABattleTankGameState * GetState() const;

};
