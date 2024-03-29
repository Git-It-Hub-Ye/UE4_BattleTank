// Copyright 2018 - 2022 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BattleHUD.generated.h"

class UPlayerWidget;
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

	/** BP widget for scoreboard */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TSubclassOf<UUserWidget> ScoreboardUI;

	/** Player ui */
	UPROPERTY()
	UPlayerWidget * PlayerWidget;

	/** Scoreboard */
	UPROPERTY()
	UScoreboardWidget * ScoreboardWidget;

public:
	ABattleHUD();


	////////////////////////////////////////////////////////////////////////////////
	// Display widgets

	/** Calls DisplayPlayerHud */
	void ShowPlayerHud(bool bDisplayThisUI);

	/** Calls DisplayInMatchWidget */
	void ShowScoreboard(bool bDisplayThisUI);

	/** Calls DisplayInMatchWidget */
	void ShowLeaderboard(bool bDisplayThisUI);


	////////////////////////////////////////////////////////////////////////////////
	// Updating to hud

	/** Update player ui of current crosshair. Request from player pawn */
	void UpdateCrosshairDisplay(bool bThirdPersonView, bool bZoomDisplay);

	/** Update player ui of current health. Request from player pawn */
	void UpdateHealthDisplay();

	/** Update player ui of damage causer position. Request from player pawn */
	void UpdateDamageIndicator(AActor * DamageCauser);

	/** Update player ui of current firing state. Request from Aiming Component */
	void UpdateWeaponStateDisplay();

	/** Update player ui when out of ammo. Request from Aiming Component */
	void WarnOutOfAmmo();

	/** Set player ui max ammo count */
	void SetMaxAmmoDisplay();

	/** Update player ui when out of match area */
	void WarnOutOfCombatArea();

	/** Remove player ui ammo warnings. Request from Aiming Component */
	void RemoveAmmoWarnings();

	/** Remove player ui warnings about combat area */
	void RemoveCombatAreaWarnings();


	////////////////////////////////////////////////////////////////////////////////
	// Updating to hud

	/** Display message about current match state */
	void UpdateMatchStateDisplay();

	/** Updates scoreboard data when score data has changed */
	void UpdateMatchScores();

private:
	////////////////////////////////////////////////////////////////////////////////
	// Display widgets

	/** Show or hide hud */
	void DisplayPlayerHud(bool bDisplayThisUI);

	/** Show or hide match display */
	void DisplayScoreboard(bool bDisplayThisUI);

	/** Show or hide match display */
	void DisplayLeaderboard(bool bDisplayThisUI);
	
};
