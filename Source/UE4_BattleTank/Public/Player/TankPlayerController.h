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
	
	/** Location for camera to aim on death */
	FVector LocationOfEnemy;


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

public:

	////////////////////////////////////////////////////////////////////////////////
	// Setup

	void ClientInGame();

	void ClientGameStarted();

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

	/** Return false if currently looking at menu to prevent some inputs */
	bool CanRecieveInput() const;

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

	/** Show scoreboard */
	void ShowLeaderboard();

	/** Hide scoreboard */
	void HideLeaderboard();

	/** Show match hud */
	void ShowMatchScoreboard();

	/** Updates match hud display */
	void UpdateMatchScoreboard();

};
