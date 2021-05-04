// Copyright 2018 to 2021 Stuart McDonald.

#include "TankPlayerState.h"
#include "Player/TankPlayerController.h"


ATankPlayerState::ATankPlayerState()
{
	Score = 0;
	KillCount = 0;
	KillAssistCount = 0;
	DeathCount = 0;
	bIsDead = false;
}

void ATankPlayerState::ScoreKill(int32 Points)
{
	KillCount++;
	AddToScore(Points);
}

void ATankPlayerState::ScoreAssist(int32 Points)
{
	KillAssistCount++;
	AddToScore(Points);
}

void ATankPlayerState::ScoreDeath()
{
	DeathCount++;
	bIsDead = true;
}

void ATankPlayerState::AddToScore(int32 ScoreDelta)
{
	Score += ScoreDelta;
}

