// Copyright 2018 - 2021 Stuart McDonald.

#include "BTDecorator_IsEnemyInRange.h"
#include "TankAIController.h"
#include "BehaviorTree/BlackboardComponent.h" 


UBTDecorator_IsEnemyInRange::UBTDecorator_IsEnemyInRange()
{
	bNotifyTick = true;
}

void UBTDecorator_IsEnemyInRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	if (!CalculateRawConditionValue(OwnerComp, NodeMemory))
	{
		OwnerComp.RequestExecution(this);
	}
}

bool UBTDecorator_IsEnemyInRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	ATankAIController * AICon = Cast<ATankAIController>(OwnerComp.GetAIOwner());

	if (AICon && BB)
	{
		auto KeyValue = BB->GetValueAsObject(EnemyKey.SelectedKeyName);
		AActor * Enemy = Cast<AActor>(KeyValue);

		if (Enemy)
		{
			FVector GetDistance = AICon->GetPawn()->GetActorLocation() - Enemy->GetActorLocation();
			float DistanceToEnemy = GetDistance.Size();

			return DistanceToEnemy <= WithinRange;
		}
		return false;
	}
	return false;
}
