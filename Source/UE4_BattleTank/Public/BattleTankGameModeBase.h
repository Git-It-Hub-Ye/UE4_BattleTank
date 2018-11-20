// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BattleTankGameModeBase.generated.h"

class APawnSpawnBox;

UCLASS()
class UE4_BATTLETANK_API ABattleTankGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

private:
	/** The bot pawn class */
	UPROPERTY(EditDefaultsOnly, Category = "Classes")
	TSubclassOf<APawn> DefaultAIBotClass;

	/** Array of spawn boxes to spawn ai within */
	TArray<APawnSpawnBox*> SpawnBoxArray;

	/** Number of AI bots to spawn in first round. For the following rounds this will be multiplied by the current round */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	int32 MaxBotsThisRound;

	/** Max AI bots alive in world at same time */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	int32 MaxBotsAtOnce;

	/** Keeps track of current amount of AI bots spawned */
	int32 CurrentNumOfBots;

	/** Current wave in progress */
	int32 CurrentWave;

	/** Time till StartWave */
	float TimeBetweenWaves;
	
	/** Timer handle for start of new wave */
	FTimerHandle NextWaveHandle;

public:
	ABattleTankGameModeBase();

	virtual void StartPlay() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	void AIBotDestroyed();
	
protected:
	/** Sets timer for new wave to start spawning ai  */
	void PrepareNewWave();

	/** Start spawning ai */
	void StartWave();

	/** Stop spawning ai */
	void EndWave();

	/** Spawns ai at empty location */
	void SpawnNewAIPawn();

private:
	/** Gets all spawn boxes in world */
	void GetSpawnLocations();
};