// Copyright 2018 - 2022 Stuart McDonald.

#include "BTTaskNode_ApplyBrakes.h"
#include "TankAIController.h"


EBTNodeResult::Type UBTTaskNode_ApplyBrakes::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	ATankAIController * AICon = Cast<ATankAIController>(OwnerComp.GetAIOwner());
	if (AICon)
	{
		AICon->ApplyBrakes(bApplyBrake);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
