// Copyright 2018 Stuart McDonald.

#include "AmmoTrigger.h"
#include "Player/Tank.h"
#include "AimingComponent.h"

void AAmmoTrigger::BeginPlay()
{
	Super::BeginPlay();
	CurrentAmmoHeld = MaxAmmoHeld;
}

void AAmmoTrigger::TriggerdBehaviour(ATank * Tank)
{
}
