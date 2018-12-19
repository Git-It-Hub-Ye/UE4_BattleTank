// Copyright 2018 Stuart McDonald.

#include "ArmourTrigger.h"
#include "Player/Tank.h"


void AArmourTrigger::OnOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	ATank * Tank = Cast<ATank>(OtherActor);

	if (Tank && Tank->IsPlayerControlled() && !Tank->IsTankDestroyed())
	{
		Tank->ReplenishArmour();
	}
}