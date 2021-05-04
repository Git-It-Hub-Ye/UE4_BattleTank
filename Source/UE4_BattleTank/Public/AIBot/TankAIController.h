// Copyright 2018 to 2021 Stuart McDonald.

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

	/** Key for enemy value */
	UPROPERTY(EditDefaultsOnly, Category = "Behaviour")
	FName EnemyKeyId;

	/** Key for line of sight value */
	UPROPERTY(EditDefaultsOnly, Category = "Behaviour")
	FName CanShootKeyId;

	/** Key for applying brakes */
	UPROPERTY(EditDefaultsOnly, Category = "Behaviour")
	FName BrakesAppliedID;

public:
	ATankAIController();

	/** Find enemies */
	UFUNCTION(BlueprintCallable)
	void FindEnemy();

	/** Fire weapons on pawn */
	void FireWeapon();

	/** Apply brakes on pawn */
	void ApplyBrakes(bool bApplyBrakes);

private:
	/** Set pawn and delegates */
	virtual void SetPawn(APawn * InPawn) override;

	/** Detach from controller on death */
	UFUNCTION()
	void OnPossessedTankDeath();
	
};
