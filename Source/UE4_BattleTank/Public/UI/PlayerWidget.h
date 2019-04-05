// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerWidget.generated.h"

class APlayerController;
class ATank;
class UAimingComponent;
class UTextBlock;
class UProgressBar;
class UImage;
class UWidgetAnimation;
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

	/** How much health player has left */
	UPROPERTY(meta = (BindWidget))
	UProgressBar * Bar_HealthRemaining;

	/** How much armour player has left */
	UPROPERTY(meta = (BindWidget))
	UProgressBar * Bar_ArmourRemaining;

	/** For aiming */
	UPROPERTY(meta = (BindWidget))
	UImage * Image_Crosshair;


	////////////////////////////////////////////////////////////////////////////////
	// Warning display

	/** Ammo warning */
	UPROPERTY(meta = (BindWidget))
	UTextBlock * Text_WarningMessage;

	/** Play area warning */
	UPROPERTY(meta = (BindWidget))
	UTextBlock * Text_WarningOutOfBounds;


	////////////////////////////////////////////////////////////////////////////////
	// Widget data

	/** Reference to player */
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	ATank * PlayerPawn;

	/** Reference to aiming component */
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	UAimingComponent * AimCompRef;


	////////////////////////////////////////////////////////////////////////////////
	// Animations

	TMap<FName, UWidgetAnimation*> AnimationsMap;

public:
	virtual bool Initialize() override;

	/** Setsup display data and animations when constructed */
	virtual void NativeConstruct() override;

	/** Initialise references */
	void InitialiseRefs();


	////////////////////////////////////////////////////////////////////////////////
	// Player data display

	/** Update ammo count and crosshair colour display */
	void UpdateWeaponDisplay();

	/** Update armour and health display */
	void UpdateHealthDisplay();


	////////////////////////////////////////////////////////////////////////////////
	// Warning display

	/** Warn when low ammo */
	void NotifyLowAmmo();

	/** Warn when no ammo */
	void NotifyOutOfAmmo();

	/** Remove warnings and stop animations */
	void RemoveAmmoWarnings();

	/** Warn when out of bounds */
	void NotifyOutOfMatchArea();
	

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
	FColor GetCrosshairColor() const;

	/** Return armour remaining */
	float GetArmourPercent() const;

	/** Return health remaining */
	float GetHealthPercent() const;


	////////////////////////////////////////////////////////////////////////////////
	// Warning display

	FText GetOutOfBoundsWarning() const;


	////////////////////////////////////////////////////////////////////////////////
	// Animations

	/** Fills a map with animations */
	void FillAnimationsMap();

	/** Returns the animation from its name */
	UWidgetAnimation * GetAnimationByName(FName AnimName) const;
	
};
