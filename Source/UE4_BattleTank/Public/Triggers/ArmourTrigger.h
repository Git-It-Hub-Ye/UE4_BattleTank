// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "Triggers/Triggers.h"
#include "ArmourTrigger.generated.h"

/**
 * Replenish armour when overlapped
 */
UCLASS()
class UE4_BATTLETANK_API AArmourTrigger : public ATriggers
{
	GENERATED_BODY()
	
protected:
	virtual void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;

};
