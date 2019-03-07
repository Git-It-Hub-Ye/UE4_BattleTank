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

public:
	ABattleTank_HordeMode();

protected:

	////////////////////////////////////////////////////////////////////////////////
	// Game behaviour

	/** Starts game mode */
	virtual void StartGame();

	/** Decrease num of active bots and check how this effects wave state */
	virtual void OnAIBotDeath(AAIController * AICon) override;

	/** Call game over if all players are dead */
	virtual void OnPlayerDeath(APlayerController * PC) override;


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

};
