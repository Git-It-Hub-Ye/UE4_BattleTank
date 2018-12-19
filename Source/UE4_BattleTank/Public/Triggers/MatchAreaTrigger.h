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
	
private:
	/** How much health is added each time repair function is called */
	UPROPERTY(EditDefaultsOnly, Category = "Config", meta = (ClampMin = 5.f, ClampMax = 30.f))
	float DeathTimer = 10.f;

	/** Timer handle for repair */
	FTimerHandle DeathTimerHandle;

	/** Actor within Trigger */
	ATank * Tank;

public:
	AMatchAreaTrigger();

protected:
	virtual void BeginPlay() override;

	/** Keeps track of what Pawns are inside area */
	virtual void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;

	/** Keeps track of pawns that leave area */
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/** Damage pawns that have'nt returned to area in time */
	void Kill();
	
};
