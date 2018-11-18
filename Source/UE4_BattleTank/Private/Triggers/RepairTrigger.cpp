// Copyright 2018 Stuart McDonald.

#include "RepairTrigger.h"
#include "Components/BoxComponent.h"
#include "Player/Tank.h"
#include "TimerManager.h"


void ARepairTrigger::BeginPlay()
{
	Super::BeginPlay();
	if (TriggerVolume == nullptr) { return; }
	TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &ARepairTrigger::OnEndOverlap);
}

void ARepairTrigger::OnOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	Tank = Cast<ATank>(OtherActor);
	if (Tank && Tank->IsPlayerControlled() && Tank->IsTankDamaged())
	{
		Repair();
	}
	return;
}

void ARepairTrigger::OnEndOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	GetWorldTimerManager().ClearTimer(RepairTimer);
}

void ARepairTrigger::Repair()
{
	GetWorldTimerManager().ClearTimer(RepairTimer);

	if (Tank && !Tank->IsTankDestroyed() && Tank->IsTankDamaged())
	{
		Tank->ReplenishHealth(HealthToAdd);
		if (Tank->IsTankDamaged())
		{
			GetWorldTimerManager().SetTimer(RepairTimer, this, &ARepairTrigger::Repair, AddRepairTime, true);
		}
	}
	return;
}
