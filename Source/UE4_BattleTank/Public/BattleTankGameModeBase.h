// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BattleTankGameModeBase.generated.h"

class ASpawnBox_Pawn;
class ASpawnBox_Actor;

UCLASS()
class UE4_BATTLETANK_API ABattleTankGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

private:
	/** The ai pawn class */
	UPROPERTY(EditDefaultsOnly, Category = "Classes")
	TSubclassOf<APawn> DefaultPawnAIClass;

	/** Array of spawn boxes to spawn ai within */
	TArray<ASpawnBox_Pawn*> BotSpawnBoxArray;

	/** Array of spawn boxes to spawn actors within */
	TArray<ASpawnBox_Actor*> ActorSpawnBoxArray;

	/** Number of AI bots to spawn in first round. For the following rounds this will be multiplied by the current round */
	UPROPERTY(EditDefaultsOnly, Category = "Config", meta = (ClampMin = 1.f, ClampMax = 20.f))
	int32 NumOfBotsToSpawn;

	/** Max AI bots alive in world at same time */
	UPROPERTY(EditDefaultsOnly, Category = "Config", meta = (ClampMin = 1.f, ClampMax = 20.f))
	int32 MaxBotsAtOnce;

	/** Max number of bots this wave */
	int32 MaxBotsThisRound;

	/** Keeps track of current amount of AI bots spawned */
	int32 TotalBotsSpawned;
	
	/** Number of bots currently active */
	int32 CurrentNumOfBotsAlive;

	/** Current wave in progress */
	int32 CurrentWave;

	/** Time till StartWave */
	float TimeBetweenWaves;
	
	/** Timer handle for start of new wave */
	FTimerHandle NextWaveHandle;

	/** Points for a kill */
	UPROPERTY(EditDefaultsOnly, Category = "Config", meta = (ClampMin = 1.f, ClampMax = 100.f))
	int32 KillPoints;

public:
	ABattleTankGameModeBase();

	virtual void StartPlay() override;

	virtual void PostLogin(APlayerController * NewPlayer) override;

	virtual void Logout(AController * Exiting) override;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	void AIBotDestroyed(APawn * AIPawn);

	void PlayerDestroyed();

	void TriggerDestroyed();

	void HandleKill(AController* KilledPawn, AController* KillerPawn);
	
protected:
	/** Sets timer for new wave to start spawning ai  */
	void PrepareNewWave();

	/** Start spawning ai */
	void StartWave();

	/** Stop spawning ai */
	void EndWave();

	/** Spawns ai at empty location */
	void SpawnNewAIPawn();

	void SpawnNewTrigger();

private:
	/** Gets all spawn boxes in world */
	void GetSpawnLocations();

	/** Updates player data on scoreboard */
	void UpdatePlayerScoreboard();

	/** Updates scoreboard of current match state */
	void UpdateMatchStateScoreboard();

	/** Update scoreboard of players current data */
	void ShowEndMatchScoreboard();

	void GameOver();

};
