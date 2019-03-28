// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoreboardWidget.generated.h"

class ATankPlayerState;
class UPanelWidget;
class UWidgetSwitcher;
class UTextBlock;
class UWidgetAnimation;
enum class EMatchState : uint8;

/** Struct for scoreboard data */
USTRUCT(BlueprintType)
struct FMatchData {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Scoreboard")
	TArray<int32> Scores;

	FText MatchMins;

	UPROPERTY(BlueprintReadOnly, Category = "Scoreboard")
	int32 MatchSecs;

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

	/** Switches between different scoreboard layouts */
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher * WidgetSwitcher;


	////////////////////////////////////////////////////////////////////////////////
	// Match display

	/** Displays match time data */
	UPROPERTY(meta = (BindWidget))
	UPanelWidget * TimerBox;

	/** Minutes remaining in match */
	UPROPERTY(meta = (BindWidget))
	UTextBlock * Text_MinsRemaining;

	/** Seconds remaining in match */
	UPROPERTY(meta = (BindWidget))
	UTextBlock * Text_SecsRemaining;

	/** Displays match round data */
	UPROPERTY(meta = (BindWidget))
	UPanelWidget * RoundsBox;

	/** Current round */
	UPROPERTY(meta = (BindWidget))
	UTextBlock * Text_CurrentRound;

	/** Holds messgae about to player */
	UPROPERTY(meta = (BindWidget))
	UPanelWidget * MessageBox;

	/** Message to player about match state */
	UPROPERTY(meta = (BindWidget))
	UTextBlock * Text_MatchStateMessage;


	////////////////////////////////////////////////////////////////////////////////
	// Match results display

	/** Holds messgae about to player */
	UPROPERTY(meta = (BindWidget))
	UPanelWidget * Panel_Results;

	/** Result of match */
	UPROPERTY(meta = (BindWidget))
	UTextBlock * Text_Result;


	////////////////////////////////////////////////////////////////////////////////
	// Leaderboard display

	/** Displays players ranked by score */
	UPROPERTY(meta = (BindWidget))
	UPanelWidget * Panel_Leaderboard;

	UPROPERTY(BlueprintReadOnly, Category = "Scoreboard")
	FMatchData MatchData;

	UPROPERTY(BlueprintReadOnly, Category = "Scoreboard")
	FLeaderboardData LeaderboardData;

	/** Player controllers sorted by score in descending */
	UPROPERTY(BlueprintReadOnly, Category = "Scoreboard")
	TArray<ATankPlayerState*> SortedPlayers;

	UPROPERTY(BlueprintReadWrite, Category = "Scoreboard")
	bool bIsLeaderboardVisible;


	////////////////////////////////////////////////////////////////////////////////
	// Animations

	TMap<FName, UWidgetAnimation*> AnimationsMap;

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Scoreboard")
	void UpdateLeaderboardData();

	/** Set values of match data */
	void UpdateMatchScoreboard();

	/** Displays mesage to user about match state */
	void UpdateMessageToUser();

	/** Show user result of match */
	void DisplayMatchResult();

	/** Show leaderboard */
	void ShowLeaderboard();

	/** Hide leaderboard */
	void HideLeaderboard();

	bool IsLeaderboardVisible() const;

protected:
	virtual bool Initialize() override;

	/** Finds what match data should be visible */
	virtual void WhatDataToDisplay();

	void PlayAnimationByName(FName AnimName);

	UFUNCTION(BlueprintCallable, Category = "Scoreboard")
	void UpdateLeaderboard(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Scoreboard")
	void SortPlayerByScores();

private:

	UFUNCTION()
	void TimerDisplay();

	void GameOverDisplay();

	FText GetStartingMatchText();

	FText GetMatchResultText() const;

	void FillAnimationsMap();

	UWidgetAnimation * GetAnimationByName(FName AnimName) const;
	
};
