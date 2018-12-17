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

	UPROPERTY(EditDefaultsOnly)
	float CrosshairXLocation = 0.5;

	UPROPERTY(EditDefaultsOnly)
	float CrosshairYLocation = 0.33333;

	UPROPERTY(EditDefaultsOnly)
	float LineTraceRange = 100000;


	////////////////////////////////////////////////////////////////////////////////
	// UI

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TSubclassOf<class UUserWidget> PlayerUI;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TSubclassOf<class UUserWidget> InGameMenu;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TSubclassOf<class UUserWidget> ScoreboardUI;

	UPlayerWidget * PlayerWidget;

	UInGameMenuWidget * InGameMenuWidget;

	UScoreboardWidget * ScoreboardWidget;

	bool bLookAtInGameMenu;

	bool bLookAtScoreboard;

public:
	ATankPlayerController();


	////////////////////////////////////////////////////////////////////////////////
	// Input

	void ATankPlayerController::SetupInputComponent();


	////////////////////////////////////////////////////////////////////////////////
	// Weapon usuage

	FVector GetCrosshairLocation() const;


	////////////////////////////////////////////////////////////////////////////////
	// UI

	void UpdateFiringStateDisplay();

	void UpdateHealthDisplay();

	void WarnOfLowAmmo(bool bLowAmmo);

	void WarnOutOfAmmo(bool bOutOfAmmo);

	void WarnOutOfMatchArea(bool bOutOfArea);

	void NotifyMenuRemoved();

	bool CanRecieveInput() const { return !bLookAtInGameMenu; }

protected:
	virtual void BeginPlay() override;

private:
	virtual void SetPawn(APawn * InPawn) override;

	UFUNCTION()
	void OnPossessedTankDeath();


	////////////////////////////////////////////////////////////////////////////////
	// Weapon usage

	// Returns an out parameter, true if hit landscape.
	FVector GetSightRayHitLocation(FVector & HitLocation) const;

	// Returns Look Direction of player crosshair.
	bool GetLookDirection(FVector2D ScreenLocation, FVector & LookDirection) const;

	// Get Hit Location from player Look Direction.
	FVector GetLookVectorHitLocation(FVector LookDirection, FVector & HitLocation) const;


	////////////////////////////////////////////////////////////////////////////////
	// UI

	void ToggleInGameMenu();

	void ShowInGameMenu();

	void HideInGameMenu();

	void ShowScoreboard();

	void HideScoreboard();

};
