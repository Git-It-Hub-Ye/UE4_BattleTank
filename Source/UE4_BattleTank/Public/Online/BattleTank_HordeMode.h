// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "Online/BattleTankGameModeBase.h"
#include "BattleTank_HordeMode.generated.h"

class AAIController;
class ASpawnBox_Pawn;

UCLASS()
class UE4_BATTLETANK_API ABattleTank_HordeMode : public ABattleTankGameModeBase
{
	GENERATED_BODY()

private:
	/** True if all players have died */
	bool bGameOver;

	////////////////////////////////////////////////////////////////////////////////
	// Default classes

	/** The ai pawn class */
	UPROPERTY(EditDefaultsOnly, Category = "Classes")
	TSubclassOf<APawn> DefaultPawnAIClass;


	////////////////////////////////////////////////////////////////////////////////
	// Bot data

	/** Number of AI bots to spawn in first round. For the following rounds this will be multiplied by the current round */
	UPROPERTY(EditDefaultsOnly, Category = "Config Bot", meta = (ClampMin = 1, ClampMax = 20))
	int32 NumOfBotsToSpawn;

	/** Max AI bots alive in world at same time */
	UPROPERTY(EditDefaultsOnly, Category = "Config Bot", meta = (ClampMin = 1, ClampMax = 20))
	int32 MaxBotAmountAtOnce;

	/** Keeps track of total AI bots spawned */
	int32 TotalBotsSpawned;

	/** Number of bots currently active */
	int32 CurrentNumOfBotsAlive;

	/** Max number of bots this wave */
	int32 MaxBotNumThisWave;


	////////////////////////////////////////////////////////////////////////////////
	// Timers

	/** Delay between round start and first bot spawned */
	UPROPERTY(EditDefaultsOnly, Category = "Config Rounds", meta = (ClampMin = 1, ClampMax = 30))
	int32 Time_DelayBotSpawn;

	/** Timer handle for starting next wave */
	FTimerHandle TimerHandle_StartWave;

	/** Timer handle for retrying spawn after failed attempts */
	FTimerHandle TimerHandle_SpawnPawnFail;

public:
	ABattleTank_HordeMode();

protected:

	////////////////////////////////////////////////////////////////////////////////
	// Game behaviour

	/** Start match */
	virtual void StartMatch() override;

	/** Decrease num of active bots and check how this effects wave state */
	virtual void OnAIBotDeath(AAIController * AICon) override;

	/** Call game over if all players are dead */
	virtual void OnPlayerDeath(APlayerController * PC) override;


	////////////////////////////////////////////////////////////////////////////////
	// Handle Waves

	/** Calls new round */
	void PrepareForNextWave();

	/** Start spawning ai */
	void StartWave();

	/** Checks if wave should continue or end */
	void CheckWaveProgress();

	/** Spawns ai at empty location */
	void SpawnNewAIPawn();

};
