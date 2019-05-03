// Copyright 2018 Stuart McDonald.

#include "AmmoTrigger.h"
#include "Engine/World.h"
#include "TimerManager.h"

#include "Tank.h"
#include "AimingComponent.h"

void AAmmoTrigger::OnOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	ATank * Tank = Cast<ATank>(OtherActor);

	if (Tank && Tank->IsPlayerControlled() && !Tank->IsTankDestroyed())
	{
		bApplyToAllPlayers ? ApplyPickupToAllPlayers(Tank) : ApplyPickupToPawn(Tank);
	}
}

void AAmmoTrigger::ApplyPickupToPawn(ATank * Tank)
{
	GetWorldTimerManager().ClearTimer(Handle_ApplyPickup);

	if (Tank && !Tank->IsTankDestroyed())
	{
		auto AimingComponent = Tank->FindComponentByClass<UAimingComponent>();
		if (AimingComponent && AimingComponent->NeedAmmo())
		{
			AimingComponent->CollectAmmo(AmountToGive);
		}
		TimerDel.BindUFunction(this, FName("ApplyPickupToPawn"), Tank);
		GetWorldTimerManager().SetTimer(Handle_ApplyPickup, TimerDel, ReapplyPickupTime, true);
	}
}

void AAmmoTrigger::ApplyPickupToAllPlayers(ATank * Tank)
{
	GetWorldTimerManager().ClearTimer(Handle_ApplyPickup);

	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		ATank * Pawn = Cast<ATank>(*It);
		if (Pawn && Pawn->IsPlayerControlled())
		{
			auto AimingComponent = Pawn->FindComponentByClass<UAimingComponent>();
			if (AimingComponent && AimingComponent->NeedAmmo())
			{
				AimingComponent->CollectAmmo(AmountToGive);
			}
		}
	}

	if (Tank && !Tank->IsTankDestroyed())
	{
		TimerDel.BindUFunction(this, FName("ApplyPickupToAllPlayers"), Tank);
		GetWorldTimerManager().SetTimer(Handle_ApplyPickup, TimerDel, ReapplyPickupTime, true);
	}
}

