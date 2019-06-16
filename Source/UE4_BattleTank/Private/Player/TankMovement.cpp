// Copyright 2018 Stuart McDonald.

#include "TankMovement.h"
#include "Kismet/GameplayStatics.h"

#include "Components/AudioComponent.h"
#include "Tank.h"
#include "Track.h"


UTankMovement::UTankMovement()
{
	PrimaryComponentTick.bCanEverTick = false;

	DriveTorquePerWheel = 1000.f;
	BrakeTorquePerWheel = 1000.f;
	TurnRate = 0.5f;
	bBrakesApplied = false;
	bIsTurning = false;
}

void UTankMovement::Initialise(UTrack * LeftTrackToSet, UTrack * RightTrackToSet)
{
	/*LeftTrack = LeftTrackToSet;
	RightTrack = RightTrackToSet;*/

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

void UTankMovement::IntendMoveForward(float Value)
{
	TankSFXPitch(FGenericPlatformMath::Abs<float>(GetMovementSpeed(Value)));

	if (Value < 0 && ForwardSpeed > 0)
	{
		ApplyBrakes(true);
	}
	else if (Value != 0)
	{
		if (bBrakesApplied == true) { ApplyBrakes(false); }
		DriveRightWheels(Value);
		DriveLeftWheels(Value);
	}
	else if (bBrakesApplied == false)
	{
		ApplyBrakes(true);
	}
}

void UTankMovement::IntendTurnRight(float Value)
{
	if (Value != 0)
	{
		if (GetOwner() == NULL) { return; }

		bIsTurning = true;

		float TurningSpeed = Value * TurnRate;
		GetOwner()->AddActorLocalRotation(FRotator(0.f, TurningSpeed, 0.f), false, nullptr, ETeleportType::TeleportPhysics);

		WheelTurnYaw += Value * WheelTurnMultiplier;

		// Don't let WheelTurnYaw get too high, reduce to zero when wheel has completed 100 full roations in a row.
		if (WheelTurnYaw == -36000.f || WheelTurnYaw == 36000.f)
		{
			WheelTurnYaw = 0.f;
		}

		LeftWheelYaw = WheelTurnYaw;
		RightWheelYaw = WheelTurnYaw * -1;
	}
	else
	{
		bIsTurning = false;
	}
}

void UTankMovement::DriveRightWheels(float Throttle)
{
	CurrentThrottle = FMath::Clamp<float>(Throttle, -1, 1);
	auto TorqueApplied = CurrentThrottle * DriveTorquePerWheel;

	if (WheelSetups.Num() > 0)
	{
		for (int32 i = FirstRightWheelElement; i < LastRightWheelElement + 1; i++)
		{
			SetDriveTorque(TorqueApplied, i);
		}
	}
}

void UTankMovement::DriveLeftWheels(float Throttle)
{
	CurrentThrottle = FMath::Clamp<float>(Throttle, -1, 1);
	auto TorqueApplied = CurrentThrottle * DriveTorquePerWheel;

	if (WheelSetups.Num() > 0)
	{
		for (int32 i = FirstLeftWheelElement; i < LastLeftWheelElement + 1; i++)
		{
			SetDriveTorque(TorqueApplied, i);
		}
	}
}

void UTankMovement::ApplyBrakes(bool bApplyBrakes)
{
	if (WheelSetups.Num() > 0)
	{
		float TorqueApplied = 0;
		if (bApplyBrakes)
		{
			TorqueApplied = BrakeTorquePerWheel;
			bBrakesApplied = true;
		}
		else
		{
			bBrakesApplied = false;
		}

		for (int32 i = 0; i < WheelSetups.Num(); i++)
		{
			SetBrakeTorque(TorqueApplied, i);
		}
	}
}

void UTankMovement::OnOwnerDeath()
{
	StopEngineSound();
}

float UTankMovement::GetMovementSpeed(float Value)
{
	if (!GetOwner()) { return Value; }

	ForwardSpeed = FVector::DotProduct(GetOwner()->GetActorForwardVector(), GetOwner()->GetVelocity());
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

