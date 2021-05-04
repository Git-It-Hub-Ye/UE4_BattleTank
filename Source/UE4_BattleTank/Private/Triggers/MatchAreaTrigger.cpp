// Copyright 2018 - 2021 Stuart McDonald.

#include "MatchAreaTrigger.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Engine/World.h"

#include "Components/BoxComponent.h"
#include "Online/BattleTankGameModeBase.h"
#include "Player/Tank.h"


AMatchAreaTrigger::AMatchAreaTrigger()
{
	TriggerVolume = CreateDefaultSubobject<UBoxComponent >(FName("Trigger Volume"));

	SetRootComponent(TriggerVolume);
	TriggerVolume->SetGenerateOverlapEvents(true);
	TriggerVolume->bHiddenInGame = true;
}

void AMatchAreaTrigger::BeginPlay()
{
	Super::BeginPlay();

	if (TriggerVolume == nullptr) { return; }
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &AMatchAreaTrigger::OnOverlap);
	TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &AMatchAreaTrigger::OnEndOverlap);
}

void AMatchAreaTrigger::OnOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	ATank * Tank = Cast<ATank>(OtherActor);
	if (Tank && Tank->IsPlayerControlled() && !Tank->IsTankDestroyed())
	{
		ABattleTankGameModeBase * const GM = GetWorld()->GetAuthGameMode<ABattleTankGameModeBase>();
		if (GM)
		{
			GM->PlayerReturnedToCombatArea(Tank);
			Tank->OutOfCombatArea(false);
		}
	}
}

void AMatchAreaTrigger::OnEndOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	ATank * Tank = Cast<ATank>(OtherActor);
	if (Tank && Tank->IsPlayerControlled() && !Tank->IsTankDestroyed())
	{
		ABattleTankGameModeBase * const GM = GetWorld()->GetAuthGameMode<ABattleTankGameModeBase>();
		if (GM)
		{
			GM->PlayerOutsideCombatArea(Tank);
			Tank->OutOfCombatArea(true);
		}
	}
}

