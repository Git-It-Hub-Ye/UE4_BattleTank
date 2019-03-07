// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoreboardWidget.generated.h"

class ATankPlayerState;

/** Struct for scoreboard data */
USTRUCT(BlueprintType)
struct FMatchData {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Scoreboard")
	TArray<int32> Scores;

	UPROPERTY(BlueprintReadOnly, Category = "Scoreboard")
	int32 MatchTime;

	/** Message about current match */
	UPROPERTY(BlueprintReadOnly, Category = "Scoreboard")
	int32 CurrentRound;

	/** Displays current match state info */
	UPROPERTY(BlueprintReadOnly, Category = "Scoreboard")
	int32 MatchStateMessage;

	/** Defaults */
	FMatchData()
	{
		CurrentRound = 0;
	}
};

/** Struct for scoreboard data */
USTRUCT(BlueprintType)
struct FLeaderboardData {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Scoreboard")
	FText PlayerName;

	UPROPERTY(BlueprintReadOnly, Category = "Scoreboard")
	int32 PlayerScore;

	UPROPERTY(BlueprintReadOnly, Category = "Scoreboard")
	int32 PlayerKills;

	UPROPERTY(BlueprintReadOnly, Category = "Scoreboard")
	int32 PlayerAssists;

	UPROPERTY(BlueprintReadOnly, Category = "Scoreboard")
	int32 PlayerDeaths;

	/** Defaults */
	FLeaderboardData()
	{
		PlayerName = FText::FromString("PlayerName");
		PlayerScore = 0;
		PlayerKills = 0;
		PlayerAssists = 0;
		PlayerDeaths = 0;
	}
};

/**
 * Displays player data on Scoreboard during game
 */
UCLASS()
class UE4_BATTLETANK_API UScoreboardWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadOnly, Category = "Scoreboard")
	FMatchData MatchData;

	UPROPERTY(BlueprintReadOnly, Category = "Scoreboard")
	FLeaderboardData LeaderboardData;

	/** Player controllers sorted by score in descending */
	UPROPERTY(BlueprintReadOnly, Category = "Scoreboard")
	TArray<ATankPlayerState*> SortedPlayers;

	UPROPERTY(BlueprintReadWrite, Category = "Scoreboard")
	bool bIsLeaderboardVisible;

public:
	/** Shows match timer */
	UFUNCTION(BlueprintImplementableEvent, Category = "Scoreboard")
	void ShowTimer();

	/** Shows match round */
	UFUNCTION(BlueprintImplementableEvent, Category = "Scoreboard")
	void ShowRounds();

	/** Update Scoreboard */
	void UpdateScoreboardData();

	/** Show new Scoreboard data */
	UFUNCTION(BlueprintImplementableEvent, Category = "Scoreboard")
	void NewScoreboardData();

	UFUNCTION(BlueprintImplementableEvent, Category = "Scoreboard")
	void UpdateLeaderboardData();

	/** Show leaderboard */
	UFUNCTION(BlueprintImplementableEvent, Category = "Scoreboard")
	void ShowLeaderboard();

	/** Hide leaderboard */
	UFUNCTION(BlueprintImplementableEvent, Category = "Scoreboard")
	void HideLeaderboard();

	bool IsLeaderboardVisible() const { return bIsLeaderboardVisible; }

protected:
	/** Finds what match data should be visible */
	UFUNCTION(BlueprintCallable, Category = "Scoreboard")
	void WhatDataToDisplay();

	UFUNCTION(BlueprintCallable, Category = "Scoreboard")
	void UpdateLeaderboard(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Scoreboard")
	void SortPlayerByScores();

private:
	/** Set values of match data */
	void SetMatchData();
	
};
