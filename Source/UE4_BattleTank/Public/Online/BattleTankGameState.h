// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BattleTankGameState.generated.h"


UENUM()
enum class EMatchState : uint8 {

	WaitingToStart,
	WaveInProgress,
	WaitingToComplete,
	WaveCompleted,
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
