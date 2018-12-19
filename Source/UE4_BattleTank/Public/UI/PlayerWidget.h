// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerWidget.generated.h"

class ATank;
class UAimingComponent;

/**
 * Display on player screen
 */
UCLASS()
class UE4_BATTLETANK_API UPlayerWidget : public UUserWidget
{
	GENERATED_BODY()


protected:
	/** Reference to player */
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	ATank * PlayerPawn;

	/** Reference to aiming component */
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	UAimingComponent * AimCompRef;

public:
	/** Initialise references */
	void InitialiseAimingComp(UAimingComponent * AimingComp, ATank * Pawn);

	/** Update weapon ui */
	void AdjustFiringDisplay();

	/** Update health ui */
	void AdjustHealthDisplay();

	/** Update when low ammo */
	void NotifyLowAmmo(bool bShowLowAmmo);

	/** Update when no ammo */
	void NotifyOutOfAmmo(bool bShowOutOfAmmo);

	/** Update when out of bounds */
	void NotifyOutOfMatchArea(bool bOutMatchArea);

protected:
	/** Shows new player ammo and crosshair colour in BP */
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void UpdateAmmoAndCrosshair();

	/** Shows new player health and armour in BP */
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void UpdateHealthAndArmour();

	/** Warns player they are low on health in BP */
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void UpdateHealthWarning(bool bShowWarning);

	/** Shows player time till death if outside match area in BP*/
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void UpdateDeathTimer(int32 RemainingTime);

	/** Warns player they are outside match area in BP*/
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void UpdateMatchAreaWarning(bool bShowWarning);

	/** Warns player they are low on ammo in BP*/
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void UpdateAmmoWarning(bool bShowWarning);

	/** Warns player they are out of ammo in BP*/
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void UpdateOutOfAmmo(bool bShowOutOfAmmo);
	
};
