// Copyright 2018 - 2021 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "PickupTrigger.h"
#include "AmmoTrigger.generated.h"

class ATank;

/**
 * Adds ammo to player when overlapped
 */
UCLASS()
class UE4_BATTLETANK_API AAmmoTrigger : public APickupTrigger
{
	GENERATED_BODY()

protected:
	/** Adds ammo to overlaping pawn */
	virtual void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;
	
	/** Apply ammo to pawn */
	UFUNCTION()
	virtual void ApplyPickupToPawn(ATank * Tank) override;

	/** Apply to all players */
	UFUNCTION()
	virtual void ApplyPickupToAllPlayers(ATank * Tank) override;

};

