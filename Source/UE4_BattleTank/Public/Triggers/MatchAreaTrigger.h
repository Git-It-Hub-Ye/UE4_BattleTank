// Copyright 2018 to 2021 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "Triggers/Triggers.h"
#include "MatchAreaTrigger.generated.h"

/**
 * Match area, a death timer will be triggered if Pawn leaves
 */
UCLASS()
class UE4_BATTLETANK_API AMatchAreaTrigger : public ATriggers
{
	GENERATED_BODY()

protected:
	/** Volume to trigger behaviour when an actor overlaps */
	UPROPERTY(VisibleDefaultsOnly)
	UBoxComponent  * TriggerVolume;

public:
	AMatchAreaTrigger();
	
protected:
	virtual void BeginPlay() override;

	/** Keeps track of what Pawns are inside area */
	virtual void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;

	/** Keeps track of pawns that leave area */
	virtual void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
	
};
