// Copyright 2018 Stuart McDonald.

#include "MatchAreaTrigger.h"
#include "Components/BoxComponent.h"
#include "Player/Tank.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"


AMatchAreaTrigger::AMatchAreaTrigger()
{
	bCanBeDamaged = false;
	ArmourVolume->bHiddenInGame = true;
	ArmourVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	DeathTimer = 10.f;
}

void AMatchAreaTrigger::BeginPlay()
{
	Super::BeginPlay();
	if (TriggerVolume == nullptr) { return; }
	TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &AMatchAreaTrigger::OnEndOverlap);
}

void AMatchAreaTrigger::OnOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	Tank = Cast<ATank>(OtherActor);
	if (Tank && Tank->IsPlayerControlled() && !Tank->IsTankDestroyed())
	{
		GetWorldTimerManager().ClearTimer(DeathTimerHandle);
	}
}

void AMatchAreaTrigger::OnEndOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	Tank = Cast<ATank>(OtherActor);
	if (Tank && Tank->IsPlayerControlled() && !Tank->IsTankDestroyed())
	{
		GetWorldTimerManager().SetTimer(DeathTimerHandle, this, &AMatchAreaTrigger::Kill, DeathTimer, false);
	}
}

void AMatchAreaTrigger::Kill()
{
	if (Tank && Tank->IsPlayerControlled() && !Tank->IsTankDestroyed())
	{
		UGameplayStatics::ApplyDamage(
			Tank,
			Tank->GetHealthPercent(),
			GetInstigatorController(),
			this,
			UDamageType::StaticClass());
	}
}

