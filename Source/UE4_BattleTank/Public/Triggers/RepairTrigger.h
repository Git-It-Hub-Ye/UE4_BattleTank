// Copyright 2018 to 2021 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "PickupTrigger.h"
#include "RepairTrigger.generated.h"

class ATank;

/**
 * Add health when ovelapped
 */
UCLASS()
class UE4_BATTLETANK_API ARepairTrigger : public APickupTrigger
{
	GENERATED_BODY()
	
protected:
	/** Gets overlapping pawn and start timer */
	virtual void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;

	/** Apply repairs to pawn */
	UFUNCTION()
	virtual void ApplyPickupToPawn(ATank * Tank) override;

	/** Apply to all players */
	UFUNCTION()
	virtual void ApplyPickupToAllPlayers(ATank * Tank) override;

};
