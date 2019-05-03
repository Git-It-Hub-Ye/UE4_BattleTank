// Copyright 2018 Stuart McDonald.

#include "ArmourTrigger.h"
#include "Engine/World.h"

#include "Player/Tank.h"


void AArmourTrigger::OnOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	ATank * Tank = Cast<ATank>(OtherActor);

	if (Tank && Tank->IsPlayerControlled() && !Tank->IsTankDestroyed())
	{
		bApplyToAllPlayers ? ApplyPickupToAllPlayers(Tank) : Tank->ReplenishArmour();
	}
}

void AArmourTrigger::ApplyPickupToAllPlayers(ATank * Tank)
{
	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		ATank * Pawn = Cast<ATank>(*It);
		if (Pawn && Pawn->IsPlayerControlled())
		{
			Pawn->ReplenishArmour();
		}
	}
}
