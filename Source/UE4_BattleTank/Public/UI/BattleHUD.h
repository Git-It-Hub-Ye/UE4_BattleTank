// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BattleHUD.generated.h"

class UPlayerWidget;
class UInGameMenuWidget;
class UScoreboardWidget;
class UMatchDisplayWidget;
class ATank;
class UAimingComponent;

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
	TSubclassOf<class UUserWidget> PlayerUI;

	/** BP widget for in game menu */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TSubclassOf<class UUserWidget> InGameMenu;

	/** BP widget for scoreboard */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TSubclassOf<class UUserWidget> ScoreboardUI;

	/** BP widget for game end ui */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TSubclassOf<class UUserWidget> MatchDisplayWidget;

	/** Player ui */
	UPlayerWidget * PlayerWidget;

	/** In game menu */
	UInGameMenuWidget * InGameMenuWidget;

	/** Scoreboard */
	UScoreboardWidget * ScoreboardWidget;

	/** Match display */
	UMatchDisplayWidget * InMacthDisplay;

public:
	ABattleHUD();

	/** Calls DisplayPlayerHud */
	void ShowPlayerHud(bool bDisplayThisUI);

	/** Calls DisplayInGameMenu */
	void ShowInGameMenu(bool bOnGameOver);

	/** Calls DisplayScoreboard */
	void ShowScoreboard(bool bDisplayThisUI);

	/** Calls DisplayInMatchWidget */
	void ShowInMatchDisplay(bool bDisplayThisUI);

	/** Remove all widget from viewport. Request from Game mode when game over */
	void RemoveWidgetsOnGameOver();

	/** Updates scoreboard data when currently being viewed and player data has changed */
	void UpdateScoreboard();

	/** Update player ui of current health. Request from player pawn */
	void UpdateHealthDisplay();

	/** Update player ui of current firing state. Request from Aiming Component */
	void UpdateFiringStateDisplay();

	/** Update player ui when low on ammo. Request from Aiming Component */
	void WarnOfLowAmmo(bool bLowAmmo);

	/** Update player ui when out of ammo. Request from Aiming Component */
	void WarnOutOfAmmo(bool bOutOfAmmo);

	/** Update player ui when out of match area. Request from Match area trigger */
	void WarnOutOfMatchArea(bool bOutOfArea);

	/** Show final result of match */
	void UpdateMatchEndDisplay();

	/** Is menu currently in player viewport */
	bool IsGameMenuInViewport();

private:
	/** Show or hide hud */
	void DisplayPlayerHud(bool bDisplayThisUI);

	/** Show or hide menu */
	void DisplayInGameMenu(bool bOnGameOver);

	/** Show or hide scoreboard */
	void DisplayScoreboard(bool bDisplayThisUI);

	/** Show or hide match display */
	void DisplayInMatchWidget(bool bDisplayThisUI);
	
};
