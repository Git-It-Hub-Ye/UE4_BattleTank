// Copyright 2018 Stuart McDonald.

#include "MatchAreaTrigger.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Engine/World.h"

#include "Online/BattleTankGameModeBase.h"
#include "Player/Tank.h"


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

