// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TankAIController.generated.h"

UCLASS()
class UE4_BATTLETANK_API ATankAIController : public AAIController
{
	GENERATED_BODY()
	
protected:
	/** Blackboard to use */
	UPROPERTY(EditDefaultsOnly, Category = "Behaviour")
	UBlackboardData * BlackboardToUse;

	/** Behaviour tree to use */
	UPROPERTY(EditDefaultsOnly, Category = "Behaviour")
	UBehaviorTree * BehaviorTreeToUse;

	/** Will be a return value */
	UPROPERTY()
	UBlackboardComponent * BB;

	// How close can AI tank get.
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float AcceptanceRadius = 8000;

	/** Key for enemy value */
	FName EnemyKeyId;

	/** Key for line of sight value */
	FName CanShootKeyId;

public:
	ATankAIController();

	/** Find enemies */
	UFUNCTION(BlueprintCallable)
	void FindEnemy();

	/** Fire weapons on pawn */
	void FireWeapon();

private:
	virtual void SetPawn(APawn * InPawn) override;

	/** Detach from controller on death */
	UFUNCTION()
	void OnPossessedTankDeath();
	
};
