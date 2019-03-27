// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TankPlayerController.generated.h"

class UAimingComponent;
class ABattleHUD;

UCLASS()
class UE4_BATTLETANK_API ATankPlayerController : public APlayerController
{
	GENERATED_BODY()
	
private:
	/** Is player in a match */
	bool bInMatch;

	/** Is in game menu in player viewport */
	bool bInGameMenuInViewport;
	
	/** Location for camera to aim on death */
	FVector LocationOfEnemy;


	////////////////////////////////////////////////////////////////////////////////
	// Weapon usuage

	/** Location along width of viewport */
	UPROPERTY(EditDefaultsOnly, Category = "Config", meta = (ClampMin = 0.0f, ClampMax = 1.f))
	float CrosshairXLocation;

	/** Location along height of viewport */
	UPROPERTY(EditDefaultsOnly, Category = "Config", meta = (ClampMin = 0.0f, ClampMax = 1.f))
	float CrosshairYLocation;

	/** Distance to perform line trace */
	UPROPERTY(EditDefaultsOnly, Category = "Config", meta = (ClampMin = 100.f, ClampMax = 100000.f))
	float LineTraceRange;

public:
	ATankPlayerController();


	////////////////////////////////////////////////////////////////////////////////
	// Setup

	void ClientInGame();

	void ClientMatchStarted();

	void ClientGameEnded();

	void ClientNotifyOfMatchState();

	void ClientRemoveWidgets();

	void EnemyThatKilledPlayer(FVector  EnemyLocation);


	////////////////////////////////////////////////////////////////////////////////
	// Input

	/** Setup player input */
	virtual void SetupInputComponent() override;


	////////////////////////////////////////////////////////////////////////////////
	// Weapon usuage

	/** Get the crosshair location to perform line trace along */
	FVector GetCrosshairLocation() const;


	////////////////////////////////////////////////////////////////////////////////
	// UI

	UAimingComponent * GetAimCompRef() const;

	/** Get Hud for this player */
	ABattleHUD * GetPlayerHud() const;

protected:
	virtual void BeginPlay() override;

private:
	/** Set pawn and delegates */
	virtual void SetPawn(APawn * InPawn) override;

	/** Deposses controlled pawn and notify Gamemode */
	UFUNCTION()
	void OnPossessedTankDeath();

	/** Rotates camera to aim at location of enemy that killed player */
	void ATankPlayerController::AimCameraAfterDeath(FVector LookAtLocation);


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

	/** Show player hud */
	void TogglePlayerHud(bool bShowHud);

	/** Toggle menu on or off */
	void ToggleInGameMenu();

	/** Removes in game menu from player viewport (Called when game ends) */
	void RemoveInGameMenu();

	/** Show scoreboard */
	void ShowLeaderboard();

	/** Hide scoreboard */
	void HideLeaderboard();

	/** Show match hud */
	void ShowMatchScoreboard();

	/** Updates match hud display */
	void UpdateMatchScoreboard();

	/** Updates and displays end result of match */
	void ShowMatchResult();

};
