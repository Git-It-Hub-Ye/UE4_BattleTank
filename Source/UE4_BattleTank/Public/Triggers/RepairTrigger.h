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
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	float HealthToAdd = 10.f;

	/** How long it takes to add repair */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	float AddRepairTime = 1.f;

	/** Timer handle for repair */
	FTimerHandle RepairTimer;

	/** Actor within Trigger */
	ATank * Tank;
	
protected:
	virtual void BeginPlay() override;

	virtual void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void Repair();

};
