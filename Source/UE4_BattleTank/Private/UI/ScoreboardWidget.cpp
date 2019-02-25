// Copyright 2018 Stuart McDonald.

#include "ScoreboardWidget.h"
#include "Player/TankPlayerController.h"
#include "Online/TankPlayerState.h"
#include "Online/BattleTankGameState.h"
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
	SortedPlayers.Empty();
	ABattleTankGameState * GS = GetOwningPlayer() ? GetOwningPlayer()->GetWorld()->GetGameState<ABattleTankGameState>() : nullptr;
	if (GS)
	{
		GS->GetRankedPlayers(SortedPlayers);
	}
}

