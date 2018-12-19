// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "Triggers/Triggers.h"
#include "AmmoTrigger.generated.h"

class ATank;

/**
 * Adds ammo to player when overlapped
 */
UCLASS()
class UE4_BATTLETANK_API AAmmoTrigger : public ATriggers
{
	GENERATED_BODY()
	
private:
	/** Max ammo amount held */
	UPROPERTY(EditDefaultsOnly, Category = "Config", meta = (ClampMin = 1, ClampMax = 100))
	int32 MaxAmmoHeld = 10;

	/** Current ammo amount held */
	int32 CurrentAmmoHeld;

protected:
	virtual void BeginPlay() override;

	/** Adds ammo to overlaping pawn */
	virtual void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;
	
};
