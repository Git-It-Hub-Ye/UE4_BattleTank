// Copyright 2018 to 2021 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_Fire.generated.h"

/**
 * Fire Tank Projectile
 */
UCLASS()
class UE4_BATTLETANK_API UBTTaskNode_Fire : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	/** Fire at enemy */
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
