// Copyright 2018 Stuart McDonald.

#include "ScoreboardWidget.h"
#include "Player/TankPlayerController.h"
#include "Player/TankPlayerState.h"
#include "Kismet/KismetMathLibrary.h"

void UScoreboardWidget::UpdateScoreboard(int32 Index)
{
	if (!SortedPlayers[Index]) { return; }
	ScoreboardData.PlayerName = SortedPlayers[Index]->GetPlayerName();
	ScoreboardData.Score = SortedPlayers[Index]->GetScore();
	ScoreboardData.Kills = SortedPlayers[Index]->GetKillCount();
	ScoreboardData.Assists = SortedPlayers[Index]->GetKillAssistCount();
	ScoreboardData.Deaths = SortedPlayers[Index]->GetDeathCount();
}

void UScoreboardWidget::SortPlayerByScores()
{
	TArray<ATankPlayerState*> AllPlayers;
	TArray<int32> AllPlayersScores;
	SortedPlayers.Empty();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController * PC = Cast<APlayerController>(*It);
		ATankPlayerState * PlayerState = PC ? Cast<ATankPlayerState>(PC->PlayerState) : nullptr;
		if (!PlayerState) { continue; }

		AllPlayers.Add(PlayerState);
		AllPlayersScores.Add(PlayerState->GetScore());
	}

	do
	{
		if (AllPlayersScores.Num() <= 0) { return; }
		int32 Item;
		int32 Score;
		UKismetMathLibrary::MaxOfIntArray(AllPlayersScores, Item, Score);

		AllPlayersScores.RemoveAt(Item);
		SortedPlayers.Add(AllPlayers[Item]);
		AllPlayers.RemoveAt(Item);
	}
	while (AllPlayers.Num() > 0);
}

