// Copyright 2018 Stuart McDonald.

#include "PlayerWidget.h"
#include "Player/Tank.h"
#include "Player/AimingComponent.h"
#include "Player/TankPlayerController.h"


void UPlayerWidget::InitialiseRefs(APlayerController * PlayerController)
{
	ATankPlayerController * PC = PlayerController ? Cast<ATankPlayerController>(PlayerController) : nullptr;
	if (PC && PC->GetPawn())
	{
		PlayerPawn = Cast<ATank>(PC->GetPawn());
		AimCompRef = PC->GetAimCompRef();
	}
}

void UPlayerWidget::AdjustFiringDisplay()
{
	if (!AimCompRef) { return; }
	UpdateAmmoAndCrosshair();
}

void UPlayerWidget::AdjustHealthDisplay()
{
	if (!PlayerPawn) { return; }
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

