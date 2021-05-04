// Copyright 2018 - 2021 Stuart McDonald.

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
	/** Allowed to use input on controlled pawns */
	bool bAllowPawnInput;

	/** Is game menu in player viewport */
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

	/** Setup any input or ui */
	void ClientInGame();

	/** Setup any input or ui */
	void ClientMatchStarted();

	/** Setup any input or ui */
	void ClientMatchFinished();

	/** Setup any input or ui */
	void ClientGameEnded();

	/** Updates client input or ui */
	void ClientNotifyOfMatchState();

	/** Cleans up any resources necessary to return to main menu */
	void HandleReturnToMainMenu();

	/** Sets a location for controller to look at */
	void LookAtLocation(FVector  EnemyLocation);


	////////////////////////////////////////////////////////////////////////////////
	// Input

	/** Setup player input */
	virtual void SetupInputComponent() override;

	/** Enables pawn input if allowed */
	void DetermineInput();


	////////////////////////////////////////////////////////////////////////////////
	// Weapon usuage

	/** Get the crosshair location to perform line trace along */
	FVector GetCrosshairLocation() const;


	////////////////////////////////////////////////////////////////////////////////
	// Controller data

	/** Returns an aiming component used by player pawn */
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
	void AimCameraAfterDeath(FVector LookAtLocation);


	////////////////////////////////////////////////////////////////////////////////
	// Setup

	/** Ends and/or destroys game session */
	void CleanupSessionOnReturnToMenu();


	////////////////////////////////////////////////////////////////////////////////
	// Weapon usage

	/** Returns an out parameter, true if hit landscape */
	FVector GetSightRayHitLocation(FVector & HitLocation) const;

	/** Returns Look Direction of player crosshair */
	bool GetLookDirection(FVector2D ScreenLocation, FVector & LookDirection) const;

	/** Get Hit Location */
	FVector GetLookVectorHitLocation(FVector LookDirection, FVector & HitLocation) const;


	////////////////////////////////////////////////////////////////////////////////
	// Input

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

	/** Remove match hud */
	void RemoveMatchScoreboard();

};
