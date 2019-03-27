// Copyright 2018 Stuart McDonald.

#include "ScoreboardWidget.h"
#include "Player/TankPlayerController.h"
#include "Online/TankPlayerState.h"
#include "Online/BattleTankGameModeBase.h"
#include "Online/BattleTankGameState.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "Internationalization/Text.h"

#define LOCTEXT_NAMESPACE "Scoreboard"


bool UScoreboardWidget::Initialize()
{
	if (!Super::Initialize()) { return false; }

	if (!ensure(Panel_Leaderboard)) { return false; }
	Panel_Leaderboard->SetVisibility(ESlateVisibility::Hidden);

	if (RoundsBox)
	{
		RoundsBox->SetVisibility(ESlateVisibility::Hidden);
	}
	if (TimerBox)
	{
		TimerBox->SetVisibility(ESlateVisibility::Hidden);
	}
	if (MessageBox)
	{
		MessageBox->SetVisibility(ESlateVisibility::Hidden);
	}

	ABattleTankGameModeBase * BTGM = Cast<ABattleTankGameModeBase>(GetWorld()->GetAuthGameMode());
	if (BTGM)
	{
		BTGM->OnTimeChanged.AddUniqueDynamic(this, &UScoreboardWidget::TimerDisplay);
	}

	WhatDataToDisplay();
	return true;
}

////////////////////////////////////////////////////////////////////////////////
// Leaderboard display

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

void UScoreboardWidget::ShowLeaderboard()
{
	if (!Panel_Leaderboard) { return; }
	Panel_Leaderboard->SetVisibility(ESlateVisibility::Visible);
	UpdateLeaderboardData();
}

void UScoreboardWidget::HideLeaderboard()
{
	if (!Panel_Leaderboard) { return; }
	Panel_Leaderboard->SetVisibility(ESlateVisibility::Hidden);
}

bool UScoreboardWidget::IsLeaderboardVisible() const
{
	return Panel_Leaderboard && Panel_Leaderboard->IsVisible();
}

void UScoreboardWidget::UpdateMatchScoreboard()
{
	ABattleTankGameState * GS = GetWorld()->GetGameState<ABattleTankGameState>();
	if (GS)
	{
		if (RoundsBox && Text_CurrentRound)
		{
			Text_CurrentRound->SetText(FText::AsNumber(GS->CurrentRound));
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
// Match display

void UScoreboardWidget::WhatDataToDisplay()
{
	ABattleTankGameState * GS = GetWorld()->GetGameState<ABattleTankGameState>();
	if (GS)
	{
		if (GS->GetIsUsingTimer())
		{
			if (TimerBox && Text_MinsRemaining && Text_SecsRemaining)
			{
				TimerBox->SetVisibility(ESlateVisibility::Visible);
			}
		}
		if (GS->GetIsUsingRounds())
		{
			if (RoundsBox && Text_CurrentRound)
			{
				RoundsBox->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
}

void UScoreboardWidget::DisplayMatchResult()
{
	UpdateMessageToUser();
}

FText UScoreboardWidget::GetMatchResultText() const
{
	FText OutComeText = FText::GetEmpty();
	ABattleTankGameState * GS = GetWorld()->GetGameState<ABattleTankGameState>();
	if (GS)
	{
		OutComeText = FText::Format(LOCTEXT("Message", "Game Over" LINE_TERMINATOR "Rounds Survived {0}"), FText::AsNumber(GS->CurrentRound - 1));
	}
	return OutComeText;
}

void UScoreboardWidget::TimerDisplay()
{
	ABattleTankGameState * GS = GetWorld()->GetGameState<ABattleTankGameState>();
	if (GS)
	{
		if (GS->GetMatchState() == EMatchState::WaitingToStart)
		{
			Text_MessageToUser->SetText(GetStartingMatchText());
		}
		else if (GS->GetMatchState() == EMatchState::InProgress)
		{
			FNumberFormattingOptions NumFormat;
			NumFormat.SetMinimumIntegralDigits(2);
			Text_MinsRemaining->SetText(FText::AsNumber(GS->RemainingTime / 60));
			Text_SecsRemaining->SetText(FText::AsNumber(GS->RemainingTime % 60, (FNumberFormattingOptions*)&NumFormat));
		}
	}
}

FText UScoreboardWidget::GetStartingMatchText()
{
	ABattleTankGameState * GS = GetWorld()->GetGameState<ABattleTankGameState>();
	if (GS)
	{
		if (GS->RemainingTime > 0)
		{
			return FText::Format(LOCTEXT("Message", "Match Begins In: {0}"), FText::AsNumber(GS->RemainingTime));
		}
		else
		{
			return LOCTEXT("Message", "Starting Match...");
		}
	}
	return FText::GetEmpty();
}

void UScoreboardWidget::UpdateMessageToUser()
{
	if (!MessageBox || !Text_MessageToUser) { return; }

	ABattleTankGameState * GS = GetWorld()->GetGameState<ABattleTankGameState>();
	if (GS)
	{
		if (GS->GetMatchState() == EMatchState::InProgress)
		{
			MessageBox->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			MessageBox->SetVisibility(ESlateVisibility::Visible);
			if (GS->GetMatchState() == EMatchState::Finished)
			{
				Text_MessageToUser->SetText(GetMatchResultText());
			}
			else if (GS->GetMatchState() == EMatchState::WaitingForPlayers)
			{
				Text_MessageToUser->SetText(LOCTEXT("Message", "Waiting For More Players..."));
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE

