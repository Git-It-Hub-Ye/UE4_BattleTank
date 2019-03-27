// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BattleHUD.generated.h"

class UPlayerWidget;
class UInGameMenuWidget;
class UScoreboardWidget;
class ATank;
class UAimingComponent;
class UUserWidget;

/**
 * Hud for game mode
 */
UCLASS()
class UE4_BATTLETANK_API ABattleHUD : public AHUD
{
	GENERATED_BODY()
	
private:
	/** BP widget for player ui */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TSubclassOf<UUserWidget> PlayerUI;

	/** BP widget for in game menu */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TSubclassOf<UUserWidget> InGameMenu;

	/** BP widget for scoreboard */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TSubclassOf<UUserWidget> ScoreboardUI;

	/** Player ui */
	UPlayerWidget * PlayerWidget;

	/** In game menu */
	UInGameMenuWidget * InGameMenuWidget;

	/** Scoreboard */
	UScoreboardWidget * ScoreboardWidget;

public:
	ABattleHUD();

	/** Calls DisplayPlayerHud */
	void ShowPlayerHud(bool bDisplayThisUI);

	/** Calls DisplayInMatchWidget */
	void ShowScoreboard(bool bDisplayThisUI);

	/** Calls DisplayInMatchWidget */
	void ShowLeaderboard(bool bDisplayThisUI);

	/** Show final match result */
	void ShowMatchResult();

	/** Remove all widget from viewport. Request from Game mode when game over */
	void RemoveWidgetsOnClient();


	////////////////////////////////////////////////////////////////////////////////
	// Updating to hud

	/** Update player ui of current health. Request from player pawn */
	void UpdateHealthDisplay();

	/** Update player ui of current firing state. Request from Aiming Component */
	void UpdateFiringStateDisplay();

	/** Display message about current match state */
	void DisplayMatchMessage();

	/** Display match result */
	void UpdateMatchResult();

	/** Updates scoreboard data when score data has changed */
	void UpdateScoreboard();

	/** Updates leaderboard data when currently being viewed and player data has changed */
	void UpdateLeaderboard();

	/** Update player ui when low on ammo. Request from Aiming Component */
	void WarnOfLowAmmo(bool bLowAmmo);

	/** Update player ui when out of ammo. Request from Aiming Component */
	void WarnOutOfAmmo(bool bOutOfAmmo);

	/** Update player ui when out of match area. Request from Match area trigger */
	void WarnOutOfMatchArea(bool bOutOfArea);

private:
	/** Show or hide hud */
	void DisplayPlayerHud(bool bDisplayThisUI);

	/** Show or hide match display */
	void DisplayScoreboard(bool bDisplayThisUI);

	/** Show or hide match display */
	void DisplayLeaderboard(bool bDisplayThisUI);
	
};
