// Copyright 2018 Stuart McDonald.

#include "TankMovement.h"
#include "Track.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

UTankMovement::UTankMovement()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTankMovement::Initialise(UTrack * LeftTrackToSet, UTrack * RightTrackToSet)
{
	LeftTrack = LeftTrackToSet;
	RightTrack = RightTrackToSet;
	TankAudio = SFXPlay(EngineLoopSfx);
}

void UTankMovement::RequestDirectMove(const FVector & MoveVelocity, bool bForceMaxSpeed)
{
	// No need for super, as we are replacing the functionality.
	auto TankForwardDirection = GetOwner()->GetActorForwardVector().GetSafeNormal();
	auto AIForwardIntention = MoveVelocity.GetSafeNormal();

	auto ForwardThrow = FVector::DotProduct(TankForwardDirection, AIForwardIntention);
	IntendMoveForward(ForwardThrow);

	auto RightThrow = FVector::CrossProduct(TankForwardDirection, AIForwardIntention).Z;
	IntendTurnRight(RightThrow);
}

void UTankMovement::IntendMoveForward(float Throw)
{
	if (!ensure(LeftTrack && RightTrack)) { return; }
	LeftTrack->SetThrottle(Throw);
	RightTrack->SetThrottle(Throw);
	TankSFXPitch(FGenericPlatformMath::Abs<float>(GetMovementSpeed(Throw)));
}

void UTankMovement::IntendTurnRight(float Throw)
{
	if (!ensure(LeftTrack && RightTrack)) { return; }
	LeftTrack->SetThrottle(Throw);
	RightTrack->SetThrottle(-Throw);
}

float UTankMovement::GetMovementSpeed(float Throw)
{
	if (!GetOwner()) { return Throw; }

	float Speed = FVector::DotProduct(GetOwner()->GetActorForwardVector(), GetOwner()->GetVelocity());
	float PitchRange = FMath::GetMappedRangeValueClamped(FVector2D(0.f, 610.f), FVector2D(MinSoundPitch, MaxSoundPitch), FGenericPlatformMath::Abs<float>(Speed));
	return PitchRange;
}

void UTankMovement::TankSFXPitch(float PitchRange)
{
	if (TankAudio)
	{
		TankAudio->SetPitchMultiplier(PitchRange);
	}
}

UAudioComponent * UTankMovement::SFXPlay(USoundBase * SoundFX)
{
	UAudioComponent * AC = nullptr;
	if (SoundFX && GetOwner())
	{
		AC = UGameplayStatics::SpawnSoundAttached(SoundFX, GetOwner()->GetRootComponent());
		AC->FadeIn(0.1f, 1.f);
	}
	return AC;
}

