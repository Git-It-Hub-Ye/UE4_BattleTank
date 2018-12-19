// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TankPlayerController.generated.h"

class UAimingComponent;
class UPlayerWidget;
class UInGameMenuWidget;
class UScoreboardWidget;

UCLASS()
class UE4_BATTLETANK_API ATankPlayerController : public APlayerController
{
	GENERATED_BODY()
	
private:
	////////////////////////////////////////////////////////////////////////////////
	// Weapon usuage

	/** Location along width of viewport */
	UPROPERTY(EditDefaultsOnly, Category = "Config", meta = (ClampMin = 0.0f, ClampMax = 1.f))
	float CrosshairXLocation = 0.5;

	/** Location along height of viewport */
	UPROPERTY(EditDefaultsOnly, Category = "Config", meta = (ClampMin = 0.0f, ClampMax = 1.f))
	float CrosshairYLocation = 0.33333;

	/** Distance to perform line trace */
	UPROPERTY(EditDefaultsOnly, Category = "Config", meta = (ClampMin = 100.f, ClampMax = 100000.f))
	float LineTraceRange = 100000;


	////////////////////////////////////////////////////////////////////////////////
	// UI

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

	/** Is player looking at game menu */
	bool bLookAtInGameMenu;

	/** Is player looking at scoreboard */
	bool bLookAtScoreboard;

public:
	ATankPlayerController();


	////////////////////////////////////////////////////////////////////////////////
	// Input

	/** Setup player input */
	void ATankPlayerController::SetupInputComponent();


	////////////////////////////////////////////////////////////////////////////////
	// Weapon usuage

	/** Get the crosshair location to perform line trace along */
	FVector GetCrosshairLocation() const;


	////////////////////////////////////////////////////////////////////////////////
	// UI

	/** Update player ui of current firing state */
	void UpdateFiringStateDisplay();

	/** Update player ui of current health */
	void UpdateHealthDisplay();

	/** Update player ui when low on ammo */
	void WarnOfLowAmmo(bool bLowAmmo);

	/** Update player ui when out of ammo */
	void WarnOutOfAmmo(bool bOutOfAmmo);

	/** Update player ui when out of match area */
	void WarnOutOfMatchArea(bool bOutOfArea);

	/** Sets bLookAtInGameMenu to false when menu removed through widget */
	void NotifyMenuRemoved();

	/** Return false if currently looking at menu to prevent some inputs */
	bool CanRecieveInput() const { return !bLookAtInGameMenu; }

protected:
	virtual void BeginPlay() override;

private:
	/** Set pawn and delegates */
	virtual void SetPawn(APawn * InPawn) override;

	/** Deposses controlled pawn and notify Gamemode */
	UFUNCTION()
	void OnPossessedTankDeath();


	////////////////////////////////////////////////////////////////////////////////
	// Weapon usage

	/** Returns an out parameter, true if hit landscape */
	FVector GetSightRayHitLocation(FVector & HitLocation) const;

	/** Returns Look Direction of player crosshair */
	bool GetLookDirection(FVector2D ScreenLocation, FVector & LookDirection) const;

	/** Get Hit Location */
	FVector GetLookVectorHitLocation(FVector LookDirection, FVector & HitLocation) const;


	////////////////////////////////////////////////////////////////////////////////
	// UI

	/** Toggle menu on or off */
	void ToggleInGameMenu();

	/** Show menu */
	void ShowInGameMenu();

	/** Hide menu */
	void HideInGameMenu();

	/** Show scoreboard */
	void ShowScoreboard();

	/** Hide scoreboard */
	void HideScoreboard();

};
