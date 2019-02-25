// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "TankPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class UE4_BATTLETANK_API ATankPlayerState : public APlayerState
{
	GENERATED_BODY()

private:
	/** Number of kills */
	int32 KillCount;

	/** Number of assisted kills */
	int32 KillAssistCount;

	/** Number of deaths */
	int32 DeathCount;

	bool bIsDead;

public:
	ATankPlayerState();

	void ScoreKill(int32 Points);

	void ScoreAssist(int32 Points);

	void ScoreDeath();

	int32 GetKillCount() const { return KillCount; }

	int32 GetKillAssistCount() const { return KillAssistCount; }

	int32 GetDeathCount() const { return DeathCount; }

	float GetScore() const { return Score; }

	bool GetIsPlayerDead() const { return bIsDead; }

private:
	void AddToScore(int32 ScoreDelta);

};
