// Copyright 2018 - 2022 Stuart McDonald.

#include "BattleTankGameState.h"
#include "TankPlayerState.h"

void ABattleTankGameState::SetMatchState(EMatchState NewState)
{
	EMatchState OldState = CurrentState;
	CurrentState = NewState;
	GameStateChanged(OldState);
}

void ABattleTankGameState::SetGameDataUsed(bool bTimer, bool bRounds)
{
	bUsingTimer = bTimer;
	bHasRounds = bRounds;
}

void ABattleTankGameState::GetRankedPlayers(TArray<ATankPlayerState*>& OutRankedPlayerState)
{
	TMultiMap<int32, ATankPlayerState*> SortedMap;

	// Get all player states and scores and add to tmap
	for (int32 i = 0; i < PlayerArray.Num(); i++)
	{
		ATankPlayerState * CurPlayerState = Cast<ATankPlayerState>(PlayerArray[i]);
		if (CurPlayerState)
		{
			SortedMap.Add(FMath::TruncToInt(CurPlayerState->GetScore()), CurPlayerState);
		}
	}

	// Sort tmap by key (highest score)
	SortedMap.KeySort(TGreater<int32>());
	OutRankedPlayerState.Empty();

	// Add player states back to OutRankedPlayerState
	for (TMultiMap<int32, ATankPlayerState*>::TIterator It(SortedMap); It; ++It)
	{
		OutRankedPlayerState.Add(It.Value());
	}
}

void ABattleTankGameState::GameStateChanged(EMatchState OldState)
{
	OnStateChanged(CurrentState, OldState);
}

