// Copyright 2018 - 2022 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "PickupTrigger.h"
#include "ArmourTrigger.generated.h"

/**
 * Replenish armour when overlapped
 */
UCLASS()
class UE4_BATTLETANK_API AArmourTrigger : public APickupTrigger
{
	GENERATED_BODY()
	
protected:
	/** Replenish armour on overlapping pawn */
	virtual void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;

	/** Apply to all players */
	virtual void ApplyPickupToAllPlayers(ATank * Tank) override;
};
