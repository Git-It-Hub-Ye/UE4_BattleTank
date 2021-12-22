// Copyright 2018 - 2021 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerWidget.generated.h"

class APlayerController;
class ATankVehicle;
class UAimingComponent;
class UTextBlock;
class UPanelWidget;
class UOverlay;
class UProgressBar;
class UImage;
class UBorder;
class UWidgetAnimation;
class UDamageArrowIndicator;
class UWidgetSwitcher;
enum class EFiringState : uint8;

/**
 * Display on player screen
 */
UCLASS()
class UE4_BATTLETANK_API UPlayerWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	////////////////////////////////////////////////////////////////////////////////
	// Player data display

	/** How much ammo player has left */
	UPROPERTY(meta = (BindWidget))
	UTextBlock * Text_AmmoRemaining;

	/** Max ammo player can carry */
	UPROPERTY(meta = (BindWidget))
	UTextBlock * Text_MaxAmmo;

	/** How much health player has left  */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_HealthRemaining;

	/** How much health player has left */
	UPROPERTY(meta = (BindWidget))
	UProgressBar * Bar_HealthRemaining;

	/** Crosshair Panel */
	UPROPERTY(meta = (BindWidget))
	UBorder * Image_Crosshair_TP;

	/** Crosshair Panel */
	UPROPERTY(meta = (BindWidget))
	UBorder * Image_Crosshair_FP;

	/** Ammo image */
	UPROPERTY(meta = (BindWidget))
	UImage * Image_Ammo;

	/** Switch between crosshairs */
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher * WS_Crosshairs;
	
	/** Third person panel */
	UPROPERTY(meta = (BindWidget))
	UPanelWidget * Panel_TPCrosshair;

	/** First person panel */
	UPROPERTY(meta = (BindWidget))
	UPanelWidget * Panel_FPCrosshair;

	/** Zoom panel */
	UPROPERTY(meta = (BindWidget))
	UPanelWidget * Panel_ZoomCrosshair;


	////////////////////////////////////////////////////////////////////////////////
	// Warning display

	/** Play are warning */
	UPROPERTY(meta = (BindWidget))
	UTextBlock * Text_WarningOutOfBounds;

	/** Warning panel */
	UPROPERTY(meta = (BindWidget))
	UPanelWidget * Panel_WarningOutOfBounds;

	/** Damage panel */
	UPROPERTY(meta = (BindWidget))
	UOverlay * Panel_Damage;


	////////////////////////////////////////////////////////////////////////////////
	// Widget data

	/** Reference to player */
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	ATankVehicle * PlayerPawn;

	/** Reference to aiming component */
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	UAimingComponent * AimCompRef;


	////////////////////////////////////////////////////////////////////////////////
	// Animations

	TMap<FName, UWidgetAnimation*> AnimationsMap;

private:
	/** Minimum digits to be shown for text formating, (eg 3 or 003)*/
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	int32 MinDigits;

	FNumberFormattingOptions NumberFormat;

	/** Arrow widget class to spawn on damage */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UDamageArrowIndicator> DmgIndicatorBlueprint;

	/** Max amount of damage arrow indicators at a time */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	int32 MaxArrowIndicators;

	/** Collection of all damage causer keys for map */
	TArray<AActor*> DamageCauserArray;

	/** Collection of all Enemies and arrow indicators currently spawned */
	TMap<AActor*, UDamageArrowIndicator*> ArrowIndicatorMap;

	/** Is low health warning enabled */
	bool bLowHealthWarning = false;

	/** Low health warning threshold */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	int32 LowHealthThreshold = 50.f;

public:
	virtual bool Initialize() override;

	/** Setsup display data and animations when constructed */
	virtual void NativeConstruct() override;

	/** Initialise references */
	void InitialiseRefs();


	////////////////////////////////////////////////////////////////////////////////
	// Player data display

	/** Sets UI Max ammo count */
	void SetMaxAmmoDisplay();

	/** Update ammo count and crosshair colour display */
	void UpdateWeaponDisplay();

	/** Update armour and health display */
	void UpdateHealthDisplay();

	/** Update player ui of current crosshair */
	void UpdateCrosshairDisplay(bool bThirdPersonView, bool bZoomDisplay);


	////////////////////////////////////////////////////////////////////////////////
	// Warning display

	/** Update damage indicator display */
	void UpdateDamageIndicators(AActor* DamageCauser);

	/** Warn when no ammo */
	void NotifyOutOfAmmo();

	/** Remove warnings and stop animations */
	void RemoveAmmoWarnings();

	/** Warn when out of bounds */
	void NotifyOutOfMatchArea();

	void RemoveCombatAreaWarning();

protected:
	////////////////////////////////////////////////////////////////////////////////
	// Animations

	/** Plays animations */
	void PlayAnimationByName(FName AnimName, float StartAtTime, int32 NumLoopsToPlay, EUMGSequencePlayMode::Type PlayMode, float PlaybackSpeed);

private:
	////////////////////////////////////////////////////////////////////////////////
	// Player data display

	/** Return ammo remaining */
	FText GetAmmoText() const;

	/** Return new crosshair color */
	void SetCrosshairColor();

	/** Add damage indicator */
	void AddDamageIndicator(AActor* DamageCauser);


	////////////////////////////////////////////////////////////////////////////////
	// Animations

	/** Fills a map with animations */
	void FillAnimationsMap();

	/** Returns the animation from its name */
	UWidgetAnimation * GetAnimationByName(FName AnimName) const;
	
};
