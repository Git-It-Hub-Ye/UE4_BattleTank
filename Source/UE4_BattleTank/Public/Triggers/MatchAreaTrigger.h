// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "Triggers/Triggers.h"
#include "MatchAreaTrigger.generated.h"

class ATank;

/**
 * Match area, a death timer will be triggered if Pawn leaves
 */
UCLASS()
class UE4_BATTLETANK_API AMatchAreaTrigger : public ATriggers
{
	GENERATED_BODY()
	
protected:
	/** Keeps track of what Pawns are inside area */
	virtual void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;

	/** Keeps track of pawns that leave area */
	virtual void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
	
};
