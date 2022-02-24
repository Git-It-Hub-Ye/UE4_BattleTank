// Copyright 2018 - 2022 Stuart McDonald.

#include "BTTaskNode_Fire.h"
#include "TankAIController.h"


EBTNodeResult::Type UBTTaskNode_Fire::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ATankAIController * AICon = Cast<ATankAIController>(OwnerComp.GetAIOwner());
	if (AICon)
	{
		AICon->FireWeapon();
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}

