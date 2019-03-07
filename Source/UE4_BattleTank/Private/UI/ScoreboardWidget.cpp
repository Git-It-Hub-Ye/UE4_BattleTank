// Copyright 2018 Stuart McDonald.

#include "ScoreboardWidget.h"
#include "Player/TankPlayerController.h"
#include "Online/TankPlayerState.h"
#include "Online/BattleTankGameState.h"
#include "Kismet/KismetMathLibrary.h"


void UScoreboardWidget::WhatDataToDisplay()
{
	ABattleTankGameState * GS = GetWorld()->GetGameState<ABattleTankGameState>();
	if (GS)
	{
		if (GS->GetIsUsingTimer())
		{
			ShowTimer();
		}
		if (GS->GetIsUsingRounds())
		{
			ShowRounds();
		}
	}
}

void UScoreboardWidget::UpdateScoreboardData()
{
	SetMatchData();
	NewScoreboardData();
}

void UScoreboardWidget::SetMatchData()
{
	ABattleTankGameState * GS = GetWorld()->GetGameState<ABattleTankGameState>();
	if (GS)
	{
		MatchData.CurrentRound = GS->GetCurrentRound();
	}
}

void UScoreboardWidget::UpdateLeaderboard(int32 Index)
{
	if (!SortedPlayers[Index]) { return; }
	LeaderboardData.PlayerName = FText::FromString(SortedPlayers[Index]->GetPlayerName());
	LeaderboardData.PlayerScore = SortedPlayers[Index]->GetScore();
	LeaderboardData.PlayerKills = SortedPlayers[Index]->GetKillCount();
	LeaderboardData.PlayerAssists = SortedPlayers[Index]->GetKillAssistCount();
	LeaderboardData.PlayerDeaths = SortedPlayers[Index]->GetDeathCount();
}

void UScoreboardWidget::SortPlayerByScores()
{
	SortedPlayers.Empty();
	ABattleTankGameState * GS = GetWorld()->GetGameState<ABattleTankGameState>();
	if (GS)
	{
		GS->GetRankedPlayers(SortedPlayers);
	}
}

