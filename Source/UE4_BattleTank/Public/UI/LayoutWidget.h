// Copyright 2018 to 2021 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LayoutWidget.generated.h"


class UTextBlock;

/**
 * Slot layout for leaderboard
 */
UCLASS()
class UE4_BATTLETANK_API ULayoutWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	/** Displays match time data */
	UPROPERTY(meta = (BindWidget))
	UTextBlock * Text_PlayerName;

	/** Displays match time data */
	UPROPERTY(meta = (BindWidget))
	UTextBlock * Text_Score;

	/** Displays match time data */
	UPROPERTY(meta = (BindWidget))
	UTextBlock * Text_Kills;

	/** Displays match time data */
	UPROPERTY(meta = (BindWidget))
	UTextBlock * Text_Assists;

	/** Displays match time data */
	UPROPERTY(meta = (BindWidget))
	UTextBlock * Text_Deaths;

public:
	void SetPlayerName(FString PlayerName);

	void SetPlayerScore(int32 Score);

	void SetPlayerKills(int32 Kills);

	void SetPlayerAssists(int32 Assists);

	void SetPlayerDeaths(int32 Deaths);
	
	
};
