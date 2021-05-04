// Copyright 2018 to 2021 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_ApplyBrakes.generated.h"

/**
 * Apply brakes to tank wheels
 */
UCLASS()
class UE4_BATTLETANK_API UBTTaskNode_ApplyBrakes : public UBTTaskNode
{
	GENERATED_BODY()
	
protected:
	/** Should apply brakes */
	UPROPERTY(EditAnywhere, Category = "Condition")
	bool bApplyBrake = false;
	
public:
	/** Apply brakes */
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
