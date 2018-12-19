// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Triggers.generated.h"

class UBoxComponent;
class USphereComponent;

UCLASS()
class UE4_BATTLETANK_API ATriggers : public AActor
{
	GENERATED_BODY()
	
protected:
	/** Volume to trigger behaviour when an actor overlaps */
	UPROPERTY(VisibleDefaultsOnly)
	UBoxComponent  * TriggerVolume;

	/** Outer shell of Trigger, used for visibility and armour */
	UPROPERTY(VisibleAnywhere)
	USphereComponent  * ArmourVolume;

	/** Armour amount at start */
	UPROPERTY(EditDefaultsOnly, Category = "Config", meta = (ClampMin = 1.f, ClampMax = 100.f))
	int32 StartingArmour;

	/** Current armour amount */
	int32 CurrentArmour;

	/** Timer to handle destruction */
	FTimerHandle DestroyTimerHandle;

public:
	ATriggers();

protected:
	virtual void BeginPlay() override;

	/** Checks for overlaps */
	UFUNCTION()
	virtual void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	/** Adjust armour when damaged */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser) override;

private:
	/** Notify gamemode when destroyed and destroy object */
	void HasBeenDestroyed();
	
};
