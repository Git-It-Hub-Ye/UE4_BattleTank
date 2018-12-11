// Copyright 2018 Stuart McDonald.

#include "PlayerWidget.h"
#include "Player/Tank.h"
#include "Player/AimingComponent.h"


void UPlayerWidget::InitialiseAimingComp(UAimingComponent * AimingComp, ATank * Pawn)
{
	AimCompRef = AimingComp;
	PlayerPawn = Pawn;
}

void UPlayerWidget::AdjustFiringDisplay()
{
	UpdateAmmoAndCrosshair();
}

void UPlayerWidget::AdjustHealthDisplay()
{
	UpdateHealthAndArmour();
}

void UPlayerWidget::NotifyLowAmmo(bool bShowLowAmmo)
{
	UpdateAmmoWarning(bShowLowAmmo);
}

void UPlayerWidget::NotifyOutOfAmmo(bool bShowOutOfAmmo)
{
	UpdateOutOfAmmo(bShowOutOfAmmo);
}

void UPlayerWidget::NotifyOutOfMatchArea(bool bOutMatchArea)
{
	UpdateMatchAreaWarning(bOutMatchArea);
}

