// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoreboardWidget.generated.h"

class ATankPlayerState;

/** Struct for scoreboard data */
USTRUCT(BlueprintType)
struct FScoreboardData {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Scoreboard")
	FString PlayerName;

	UPROPERTY(BlueprintReadOnly, Category = "Scoreboard")
	int32 Score;

	UPROPERTY(BlueprintReadOnly, Category = "Scoreboard")
	int32 Kills;

	UPROPERTY(BlueprintReadOnly, Category = "Scoreboard")
	int32 Assists;

	UPROPERTY(BlueprintReadOnly, Category = "Scoreboard")
	int32 Deaths;

	/** Defaults */
	FScoreboardData()
	{
		PlayerName = "PlayerName";
		Score = 0;
		Kills = 0;
		Assists = 0;
		Deaths = 0;
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
	FScoreboardData ScoreboardData;

	/** Player controllers sorted by score in descending */
	UPROPERTY(BlueprintReadOnly, Category = "Scoreboard")
	TArray<ATankPlayerState*> SortedPlayers;

	UPROPERTY(BlueprintReadWrite, Category = "Scoreboard")
	bool bIsLeaderboardVisible;

public:
	/** Update leaderboard */
	UFUNCTION(BlueprintImplementableEvent, Category = "Scoreboard")
	void UpdateData();

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
	UFUNCTION(BlueprintCallable, Category = "Scoreboard")
	void UpdateScoreboard(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Scoreboard")
	void SortPlayerByScores();
	
};
