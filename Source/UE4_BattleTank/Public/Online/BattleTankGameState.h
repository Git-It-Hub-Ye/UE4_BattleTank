// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BattleTankGameState.generated.h"


UENUM()
enum class EMatchState : uint8 {

	WaitingToStart,
	InProgress,
	WaitingToComplete,
	Completed,
	GameOver
};


UCLASS()
class UE4_BATTLETANK_API ABattleTankGameState : public AGameStateBase
{
	GENERATED_BODY()
	
protected:
	/** Current state of game */
	UPROPERTY(BlueprintReadOnly, Category = "GameState")
	EMatchState CurrentState;

private:
	/** number of teams in current game (doesn't deprecate when no players are left in a team) */
	int32 NumTeams;

	/** accumulated score per team */
	TArray<int32> TeamScores;

	/** time left for warmup / match */
	int32 RemainingTime;

	/** is timer paused? */
	bool bTimerPaused;

public:
	/** Sets the current state */
	void SetMatchState(EMatchState NewState);

	/** Sorts player states by score for this match */
	void GetRankedPlayers(TArray<class ATankPlayerState*> & OutRankedPlayerState);

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "GameState")
	void OnStateChanged(EMatchState NewState, EMatchState OldState);

private:
	void GameStateChanged(EMatchState OldState);
	
};
