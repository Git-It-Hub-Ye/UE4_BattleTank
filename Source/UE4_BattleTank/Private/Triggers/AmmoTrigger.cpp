// Copyright 2018 Stuart McDonald.

#include "AmmoTrigger.h"
#include "Tank.h"
#include "AimingComponent.h"

void AAmmoTrigger::BeginPlay()
{
	Super::BeginPlay();
	CurrentAmmoHeld = MaxAmmoHeld;
}

void AAmmoTrigger::OnOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (CurrentAmmoHeld <= 0) { return; }

	ATank * Tank = Cast<ATank>(OtherActor);

	if (Tank && Tank->IsPlayerControlled() && !Tank->IsTankDestroyed())
	{
		auto AimingComponent = Tank->FindComponentByClass<UAimingComponent>();
		if (AimingComponent && AimingComponent->NeedAmmo())
		{
			int32 AmmoToAdd = FMath::Clamp<int32>(AimingComponent->GetAmmoDifference(), 0, CurrentAmmoHeld);
			AimingComponent->CollectAmmo(AmmoToAdd);
			CurrentAmmoHeld -= AmmoToAdd;
			if (CurrentAmmoHeld <= 0) { Destroy(); }
		}
	}
	return;
}

