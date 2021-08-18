// Copyright 2018 - 2021 Stuart McDonald.

#include "TankAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Online/BattleTankGameModeBase.h"
#include "Tank.h"
#include "Player/AimingComponent.h"


ATankAIController::ATankAIController()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATankAIController::SetPawn(APawn * InPawn)
{
	Super::SetPawn(InPawn);
	if (InPawn)
	{
		auto PossessedTank = Cast<ATank>(InPawn);

		if (!PossessedTank) { return; }
		PossessedTank->OnDeath.AddUniqueDynamic(this, &ATankAIController::OnPossessedTankDeath);

		if (BlackboardToUse)
		{
			UseBlackboard(BlackboardToUse, BB);
			BB->SetValueAsBool(CanShootKeyId, true);
		}
		if (BehaviorTreeToUse)
		{
			RunBehaviorTree(BehaviorTreeToUse);
		}
	}
}

void ATankAIController::FindEnemy()
{
	auto AIPawn = GetPawn();
	if (!AIPawn) { return; }

	const FVector MyLoc = AIPawn->GetActorLocation();
	float BestDistSq = MAX_FLT;
	ATank * BestEnemy = nullptr;

	// Get all Pawns in world
	for (FConstPawnIterator i = GetWorld()->GetPawnIterator(); i; i++)
	{
		// Is pawn controlled by a player
		ATank * EnemyPawn = Cast<ATank>(*i);
		if (EnemyPawn && !EnemyPawn->IsTankDestroyed() && EnemyPawn->IsPlayerControlled())
		{
			// If this Player is closest to AI Pawn set as enemy
			const float DistSq = (EnemyPawn->GetActorLocation() - MyLoc).SizeSquared();
			if (DistSq < BestDistSq)
			{
				BestDistSq = DistSq;
				BestEnemy = EnemyPawn;
			}
		}
	}
	if (BestEnemy && BB)
	{
		BB->SetValueAsObject(EnemyKeyId, BestEnemy);
	}
	else if (BB)
	{
		BB->ClearValue(EnemyKeyId);
	}
}

void ATankAIController::FireWeapon()
{
	ATank * AIBot = Cast<ATank>(GetPawn());
	ATank * Enemy = Cast<ATank>(BB->GetValueAsObject(EnemyKeyId));
	bool bCanShoot = false;
	if (AIBot && Enemy)
	{
		if (LineOfSightTo(Enemy, AIBot->GetActorLocation()))
		{
			bCanShoot = true;
			BB->SetValueAsBool(CanShootKeyId, true);
		}
		else
		{
			BB->SetValueAsBool(CanShootKeyId, false);
		}

		// Aim at player.
		auto AimComponent = AIBot->FindComponentByClass<UAimingComponent>();
		if (AimComponent)
		{
			AimComponent->AimAt(Enemy->GetActorLocation());
			if (bCanShoot && !AimComponent->IsBarrelMoving())
			{
				AimComponent->FireProjectile();
			}
		}
	}
}

void ATankAIController::ApplyBrakes(bool bApplyBrake)
{
	ATank * AIPawn = Cast<ATank>(GetPawn());

	if (AIPawn == NULL) { return; }

	BB->SetValueAsBool(BrakesAppliedID, bApplyBrake);
}

void ATankAIController::OnPossessedTankDeath()
{
	if (!GetPawn()) { return; }
	ABattleTankGameModeBase * BTGM = Cast<ABattleTankGameModeBase>(GetWorld()->GetAuthGameMode());

	if (BTGM)
	{
		BTGM->OnAIBotDeath(this);
	}
	GetPawn()->DetachFromControllerPendingDestroy();
}

