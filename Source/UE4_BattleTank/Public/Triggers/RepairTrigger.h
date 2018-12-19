// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "Triggers/Triggers.h"
#include "RepairTrigger.generated.h"

class ATank;

/**
 * Add health when ovelapped
 */
UCLASS()
class UE4_BATTLETANK_API ARepairTrigger : public ATriggers
{
	GENERATED_BODY()

private:
	/** How much health is added each time repair function is called */
	UPROPERTY(EditDefaultsOnly, Category = "Config", meta = (ClampMin = 1.f, ClampMax = 100.f))
	float HealthToAdd = 10.f;

	/** How long it takes to add repair */
	UPROPERTY(EditDefaultsOnly, Category = "Config", meta = (ClampMin = 0.1f, ClampMax = 10.f))
	float AddRepairTime = 1.f;

	/** Timer handle for repair */
	FTimerHandle RepairTimer;

	/** Actor within Trigger */
	ATank * Tank;
	
protected:
	virtual void BeginPlay() override;

	/** Gets overlapping pawn and start timer */
	virtual void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;

	/** End timer when pawn leaves area */
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/** Add repairs to pawn */
	void Repair();

};
