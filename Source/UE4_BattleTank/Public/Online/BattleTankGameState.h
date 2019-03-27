// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BattleTankGameState.generated.h"


UENUM()
enum class EMatchState : uint8 {

	WaitingForPlayers,
	WaitingToStart,
	InProgress,
	WaitingToComplete,
	Completed,
	Finished
};


UCLASS()
class UE4_BATTLETANK_API ABattleTankGameState : public AGameStateBase
{
	GENERATED_BODY()
	
protected:
	/** Current state of game */
	UPROPERTY(BlueprintReadOnly, Category = "GameState")
	EMatchState CurrentState;

public:
	/** number of teams in current game (doesn't deprecate when no players are left in a team) */
	int32 NumTeams;

	/** Accumulated score per team */
	TArray<int32> TeamScores;

	/** time left for warmup / match */
	int32 RemainingTime;

	/** Current round of match */
	int32 CurrentRound;

	/** is timer paused? */
	bool bTimerPaused;

	/** Game has timer */
	bool bUsingTimer;

	/** Game has rounds */
	bool bHasRounds;

public:
	/** Sets the current state */
	void SetMatchState(EMatchState NewState);

	/** Sets data used by this game (eg, is there rounds or a timer etc) */
	void SetGameDataUsed(bool bTimer, bool bRounds);

	/** Is game using a timer */
	bool GetIsUsingTimer() const { return bUsingTimer; }

	/** Is game using rounds */
	bool GetIsUsingRounds() const { return bHasRounds; }

	/** Sorts player states by score for this match */
	void GetRankedPlayers(TArray<class ATankPlayerState*> & OutRankedPlayerState);

	EMatchState GetMatchState() const { return CurrentState; }

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "GameState")
	void OnStateChanged(EMatchState NewState, EMatchState OldState);

private:
	void GameStateChanged(EMatchState OldState);
	
};
