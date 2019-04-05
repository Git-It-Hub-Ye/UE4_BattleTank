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
class ULayoutWidget;
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

struct FColumnData {
	/** Column name */
	FText Name;

	/** Column color */
	FColor Color;

	/** defaults */
	FColumnData()
	{
		Color = FColor(0.f, 0.44f, 1.f, 0.3f);
	}

	FColumnData(FText InName, FColor InColor)
		: Name(InName)
		, Color(InColor)
	{
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
	UPanelWidget * Panel_LeaderboardGrid;

	/** Holds player data slots */
	UPROPERTY(meta = (BindWidget))
	UPanelWidget * Panel_LayoutSlots;

	UPROPERTY(BlueprintReadOnly, Category = "Scoreboard")
	FMatchData MatchData;

	UPROPERTY(BlueprintReadOnly, Category = "Scoreboard")
	FLeaderboardData LeaderboardData;

	/** Player controllers sorted by score in descending */
	UPROPERTY(BlueprintReadOnly, Category = "Scoreboard")
	TArray<ATankPlayerState*> SortedPlayers;

	TArray<FColumnData> Columns;

	bool bShowMatchResults;


	////////////////////////////////////////////////////////////////////////////////
	// Animations

	TMap<FName, UWidgetAnimation*> AnimationsMap;

private:
	////////////////////////////////////////////////////////////////////////////////
	// Leaderboard display

	/** BP widget layout for leaderboad slots */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TSubclassOf<UUserWidget> LeaderboadSlot;

	/** Slot layout to display player data */
	ULayoutWidget * SlotLayoutWidget;

	/** Array of slots added to leaderboard */
	TArray<ULayoutWidget*> Slots;

public:
	////////////////////////////////////////////////////////////////////////////////
	// Scoreboard display

	/** Displays mesage to user about match state */
	void UpdateMatchStateDisplay();

	/** Display match score */
	void UpdateMatchScore();

	////////////////////////////////////////////////////////////////////////////////
	// Leaderboard display

	/** Show leaderboard */
	void ShowLeaderboard();

	/** Hide leaderboard */
	void HideLeaderboard();

	/** Updates leaderboard if currently visible */
	void UpdateLeaderboard();

protected:
	/** Finds widgets that exist and setsup for display */
	virtual bool Initialize() override;

	/** Setsup display data and animations when constructed */
	virtual void NativeConstruct() override;

	/** Finds what match data should be visible */
	virtual void WhatDataToDisplay();


	////////////////////////////////////////////////////////////////////////////////
	// Animations

	/** Plays animations */
	void PlayAnimationByName(FName AnimName);

private:
	////////////////////////////////////////////////////////////////////////////////
	// Scoreboard display

	/** Updates timer display */
	UFUNCTION()
	void TimerDisplay();

	/** Displays time till match starts */
	FText GetStartingMatchText();

	/** Displays match end panel */
	void GameOverDisplay();

	/** Displays end result to user */
	FText GetMatchResultText() const;


	////////////////////////////////////////////////////////////////////////////////
	// Leaderboard display

	/** Updates leaderboard */
	void FillMatchLeaderboard();

	/** Updates round display */
	void UpdateCurrentRound();

	/** Fills array of player states ranked by score */
	void SortPlayerByScores();


	////////////////////////////////////////////////////////////////////////////////
	// Animations

	/** Fills a map with animations */
	void FillAnimationsMap();

	/** Returns the animation from its name */
	UWidgetAnimation * GetAnimationByName(FName AnimName) const;
	
};
