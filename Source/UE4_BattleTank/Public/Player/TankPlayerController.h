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
