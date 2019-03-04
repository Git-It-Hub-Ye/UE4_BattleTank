// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BattleTankGameModeBase.generated.h"

class AAIController;
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

	/** Triggers to spawn in world */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TArray<TSubclassOf<AActor>> TriggerArray;


	////////////////////////////////////////////////////////////////////////////////
	// Gamemode data

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

	/** Does this game mode spawn bots */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	bool bAllowBots;

	/** Does this game mode spawn bots */
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

	virtual void StartPlay() override;

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
	// Handle Game changes

	/** Set current state of game */
	void SetGameState(EMatchState NewState);

	/** Ends game */
	void GameOver();

	/** Spawns ne trigger in world */
	void SpawnNewTrigger();

	////////////////////////////////////////////////////////////////////////////////
	// World data

	/** Gets all spawn boxes in world */
	void GetSpawnLocations();

private:

	////////////////////////////////////////////////////////////////////////////////
	// Handle Game changes

	/** Updates player scoreboard */
	void UpdatePlayerScoreboard();

	/** Updates player scoreboard */
	void EndMatchScoreboard();

};
