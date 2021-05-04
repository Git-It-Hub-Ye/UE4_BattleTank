// Copyright 2018 - 2021 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_IsEnemyInRange.generated.h"

/**
 * Is enemy in range of AI pawn
 */
UCLASS()
class UE4_BATTLETANK_API UBTDecorator_IsEnemyInRange : public UBTDecorator
{
	GENERATED_BODY()
	
protected:
	/** Value of current enemy */
	UPROPERTY(EditAnywhere, Category = "Condition")
	struct FBlackboardKeySelector EnemyKey;

	/** Target radius for ai to enemy */
	UPROPERTY(EditAnywhere, Category = "Condition")
	float WithinRange;

public:
	UBTDecorator_IsEnemyInRange();

	/** Notify when to check for enemy within radius */
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	/** Checks if ai is within a certain radius to enemy */
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	
};
