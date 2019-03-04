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
	////////////////////////////////////////////////////////////////////////////////
	// Default classes

	/** The ai pawn class */
	UPROPERTY(EditDefaultsOnly, Category = "Classes")
	TSubclassOf<APawn> DefaultPawnAIClass;


	////////////////////////////////////////////////////////////////////////////////
	// GameMode bot data

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


	////////////////////////////////////////////////////////////////////////////////
	// GameMode data

	/** Current wave in progress */
	int32 CurrentWave;

public:
	ABattleTank_HordeMode();

protected:
	virtual void PostLogin(APlayerController * NewPlayer) override;

	
	////////////////////////////////////////////////////////////////////////////////
	// Handle Waves

	/** Sets timer for new wave to start spawning ai  */
	void PrepareNewWave();

	/** Start spawning ai */
	void StartWave();

	/** Checks if wave should continue or end */
	void CheckWaveProgress();

	/** Stop spawning ai */
	void EndWave();


	////////////////////////////////////////////////////////////////////////////////
	// Game behaviour

	/** Decrease num of active bots and check how this effects wave state */
	virtual void OnAIBotDeath(AAIController * AICon) override;

	/** Call game over if all players are dead */
	virtual void OnPlayerDeath(APlayerController * PC) override;

	/** Spawns ai at empty location */
	void SpawnNewAIPawn();

};
