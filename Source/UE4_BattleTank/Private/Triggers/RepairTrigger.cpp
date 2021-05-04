// Copyright 2018 to 2021 Stuart McDonald.

#include "RepairTrigger.h"
#include "Engine/World.h"
#include "TimerManager.h"

#include "Components/BoxComponent.h"
#include "Player/Tank.h"


void ARepairTrigger::OnOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	ATank * Tank = Cast<ATank>(OtherActor);
	if (Tank && Tank->IsPlayerControlled() && !Tank->IsTankDestroyed())
	{
		bApplyToAllPlayers ? ApplyPickupToAllPlayers(Tank) : ApplyPickupToPawn(Tank);
	}
}

void ARepairTrigger::ApplyPickupToPawn(ATank * Tank)
{
	GetWorldTimerManager().ClearTimer(Handle_ApplyPickup);

	if (Tank && !Tank->IsTankDestroyed() && Tank->IsTankDamaged())
	{
		Tank->ReplenishHealth(AmountToGive);
		TimerDel.BindUFunction(this, FName("ApplyPickupToPawn"), Tank);
		GetWorldTimerManager().SetTimer(Handle_ApplyPickup, TimerDel, ReapplyPickupTime, true);
	}
}

void ARepairTrigger::ApplyPickupToAllPlayers(ATank * Tank)
{
	GetWorldTimerManager().ClearTimer(Handle_ApplyPickup);

	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		ATank * Pawn = Cast<ATank>(*It);
		if (Pawn && Pawn->IsPlayerControlled())
		{
			Pawn->ReplenishHealth(AmountToGive);
		}
	}

	if (Tank && !Tank->IsTankDestroyed())
	{
		TimerDel.BindUFunction(this, FName("ApplyPickupToAllPlayers"), Tank);
		GetWorldTimerManager().SetTimer(Handle_ApplyPickup, TimerDel, ReapplyPickupTime, true);
	}
}
