// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "Triggers/Triggers.h"
#include "PickupTrigger.generated.h"


/**
 * When triggered applies behaviour to pawns
 */
UCLASS()
class UE4_BATTLETANK_API APickupTrigger : public ATriggers
{
	GENERATED_BODY()
	
protected:
	/** Volume to trigger behaviour when an actor overlaps */
	UPROPERTY(VisibleAnywhere)
	USphereComponent  * TriggerVolume;

	/** Amount added each time ApplyPickup function is called (eg- Ammo, Health etc)*/
	UPROPERTY(EditDefaultsOnly, Category = "Config", meta = (ClampMin = 1.f, ClampMax = 100.f))
	float AmountToGive = 10.f;

	/** How long between each application */
	UPROPERTY(EditDefaultsOnly, Category = "Config", meta = (ClampMin = 0.1f, ClampMax = 10.f))
	float ReapplyPickupTime = 1.f;

	/** Does trigger affect overlaped player or all team mates */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	bool bApplyToAllPlayers;

	/** Outer shell of Trigger, used for visibility and armour */
	UPROPERTY(VisibleAnywhere)
	USphereComponent  * ArmourVolume;

	/** Armour amount at start */
	UPROPERTY(EditDefaultsOnly, Category = "Config", meta = (ClampMin = 1.f, ClampMax = 100.f))
	int32 StartingArmour;

	/** Current armour amount */
	int32 CurrentArmour;

	/** Timer handle for applying pickup behaviour */
	FTimerHandle Handle_ApplyPickup;

	/** Calls function with parameters by a timer */
	FTimerDelegate TimerDel;

public:
	APickupTrigger();

protected:
	virtual void BeginPlay() override;

	/** Adjust armour when damaged */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser) override;

	/** Apply to pawn */
	virtual void ApplyPickupToPawn(ATank * Tank) PURE_VIRTUAL(APickupTrigger::ApplyPickupToPawn, )

	/** Apply to all players */
	virtual void ApplyPickupToAllPlayers(ATank * Tank) PURE_VIRTUAL(APickupTrigger::ApplyPickupToAllPlayers, )

private:
	/** End timer when pawn leaves area */
	virtual void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	/** Notify gamemode when destroyed and destroy object */
	void HasBeenDestroyed();
	
};
