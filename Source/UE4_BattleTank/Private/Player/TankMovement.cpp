// Copyright 2018 Stuart McDonald.

#include "TankMovement.h"
#include "Kismet/GameplayStatics.h"

#include "Components/AudioComponent.h"
#include "Tank.h"
#include "Track.h"


UTankMovement::UTankMovement()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTankMovement::Initialise(UTrack * LeftTrackToSet, UTrack * RightTrackToSet)
{
	LeftTrack = LeftTrackToSet;
	RightTrack = RightTrackToSet;

	EngineAudio = SFXPlay(EngineLoopSfx);

	if (GetOwner() != NULL)
	{
		ATank * OwnerPawn = Cast<ATank>(GetOwner());

		if (OwnerPawn == NULL) { return; }
		OwnerPawn->OnDeath.AddUniqueDynamic(this, &UTankMovement::OnOwnerDeath);
	}
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
	TankSFXPitch(FGenericPlatformMath::Abs<float>(GetMovementSpeed(Throw)));

	if (!ensure(LeftTrack && RightTrack)) { return; }
	if (Throw == 0.f && bTurningRight) { return; }
	LeftTrack->SetThrottle(Throw, bTurningRight);
	RightTrack->SetThrottle(Throw, bTurningRight);
}

void UTankMovement::IntendTurnRight(float Throw)
{
	if (!ensure(LeftTrack && RightTrack)) { return; }
	if (Throw == 0.f) { bTurningRight = false; return; }
	
	bTurningRight = true;
	LeftTrack->SetThrottle(Throw, bTurningRight);
	RightTrack->SetThrottle(-Throw, bTurningRight);
}

void UTankMovement::OnOwnerDeath()
{
	StopEngineSound();
}

float UTankMovement::GetMovementSpeed(float Throw)
{
	if (!GetOwner()) { return Throw; }

	float ForwardSpeed = FVector::DotProduct(GetOwner()->GetActorForwardVector(), GetOwner()->GetVelocity());
	float TurningSpeed = FVector::DotProduct(GetOwner()->GetActorRightVector(), GetOwner()->GetVelocity());

	float TurningPitchRange = FMath::GetMappedRangeValueClamped(FVector2D(0.f, 100.f), FVector2D(MinSoundPitch, MaxTurnSoundPitch), FGenericPlatformMath::Abs<float>(TurningSpeed));
	float TotalPitchRange = FMath::GetMappedRangeValueClamped(FVector2D(0.f, 650.f), FVector2D(MinSoundPitch, MaxSoundPitch), FGenericPlatformMath::Abs<float>(ForwardSpeed));
	return TotalPitchRange < TurningPitchRange ? TurningPitchRange : TotalPitchRange;
}

void UTankMovement::TankSFXPitch(float PitchRange)
{
	if (EngineAudio)
	{
		EngineAudio->SetPitchMultiplier(PitchRange);
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

void UTankMovement::StopEngineSound()
{
	if (EngineAudio && EngineAudio->IsPlaying())
	{
		EngineAudio->Stop();
	}
}

float UTankMovement::GetRightTrackWheelSpeed() const
{
	if (!RightTrack) { return 0.f; }
	return RightTrack->GetFrontAndRearWheelSpeed();
}

float UTankMovement::GetLeftTrackWheelSpeed() const
{
	if (!LeftTrack) { return 0.f; }
	return LeftTrack->GetFrontAndRearWheelSpeed();
}

