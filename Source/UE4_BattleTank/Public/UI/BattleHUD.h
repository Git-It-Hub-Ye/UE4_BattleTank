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

	/** Player ui */
	UPlayerWidget * PlayerWidget;

	/** In game menu */
	UInGameMenuWidget * InGameMenuWidget;

	/** Scoreboard */
	UScoreboardWidget * ScoreboardWidget;

public:
	ABattleHUD();

	/** Show or hide hud. Request from player controller */
	void ShowPlayerHud(bool bDisplayThisUI);

	/** Show or hide menu. Request from player controller */
	void ShowInGameMenu(bool bOnGameOver);

	/** Show or hide scoreboard. Request from player controller */
	void ShowScoreboard(bool bDisplayThisUI);

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

	/** Is menu currently in player viewport */
	bool IsGameMenuInViewport();

private:
	/** Show or hide hud */
	void DisplayPlayerHud(bool bDisplayThisUI);

	/** Show or hide menu */
	void DisplayInGameMenu(bool bOnGameOver);

	/** Show or hide scoreboard */
	void DisplayScoreboard(bool bDisplayThisUI);
	
};
