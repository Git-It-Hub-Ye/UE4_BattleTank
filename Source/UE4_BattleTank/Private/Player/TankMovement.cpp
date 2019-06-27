// Copyright 2018 Stuart McDonald.

#include "TankMovement.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

#include "Components/AudioComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Tank.h"


UTankMovement::UTankMovement()
{
	PrimaryComponentTick.bCanEverTick = false;

	DriveTorquePerWheel = 1000.f;
	BrakeTorquePerWheel = 1000.f;
	TurnRate = 0.5f;
	bBrakesApplied = false;
}

void UTankMovement::BeginPlay()
{
	EngineAudio = SFXPlay(EngineLoopSfx);

	if (GetOwner() != NULL)
	{
		ATank * OwnerPawn = Cast<ATank>(GetOwner());

		if (OwnerPawn == NULL) { return; }
		OwnerPawn->OnDeath.AddUniqueDynamic(this, &UTankMovement::OnOwnerDeath);
	}
}


////////////////////////////////////////////////////////////////////////////////
// Drive tank

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
	MoveForwardValue = FMath::Clamp<float>(Value, -0.5f, 1.f);

	if (MoveForwardValue != 0)
	{
		if (bBrakesApplied == true) { ApplyBrakes(false); }
		DriveRightWheels(MoveForwardValue);
		DriveLeftWheels(MoveForwardValue);
	}
	else if (bBrakesApplied == false)
	{
		ApplyBrakes(true);
	}

	ApplyMovementSpeedBehaviours();
}

void UTankMovement::IntendTurnRight(float Value)
{
	TurnRightValue = FMath::Clamp<float>(Value, -1, 1);

	if (TurnRightValue != 0)
	{
		if (GetOwner() == NULL) { return; }

		float CurrentThrottle = TurnRightValue * TurnRate * GetWorld()->DeltaTimeSeconds;

		TurnSpeed += CurrentThrottle;
		TurnSpeed = FMath::Clamp<float>(TurnSpeed, -TurnRate, TurnRate);

		float TurningSpeed = FMath::Clamp<float>(TurnSpeed, -TurnRate, TurnRate);

		GetOwner()->AddActorLocalRotation(FRotator(0.f, TurningSpeed, 0.f), false, nullptr, ETeleportType::TeleportPhysics);
	}
	else if (TurnSpeed != 0.f)
	{
		TurnSpeed = 0;
	}
}

void UTankMovement::DriveRightWheels(float Throttle)
{
	float CurrentThrottle = FMath::Clamp<float>(Throttle, -1, 1);
	auto TorqueApplied = CurrentThrottle * DriveTorquePerWheel;

	if (WheelSetups.Num() > 0)
	{
		for (int32 i = FirstRightWheelIndex; i < LastRightWheelIndex + 1; i++)
		{
			SetDriveTorque(TorqueApplied, i);
		}
	}
}

void UTankMovement::DriveLeftWheels(float Throttle)
{
	float CurrentThrottle = FMath::Clamp<float>(Throttle, -1, 1);
	auto TorqueApplied = CurrentThrottle * DriveTorquePerWheel;

	if (WheelSetups.Num() > 0)
	{
		for (int32 i = FirstLeftWheelIndex; i < LastLeftWheelIndex + 1; i++)
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

void UTankMovement::ApplyMovementSpeedBehaviours()
{
	float EnginePitch = MoveForwardValue;
	float ForwardSpeed = MoveForwardValue;
	float TurningSpeed = TurnRightValue;

	if (GetOwner())
	{
		float ForwardVelocity = FVector::DotProduct(GetOwner()->GetActorForwardVector(), GetOwner()->GetVelocity());
		EnginePitch = FMath::GetMappedRangeValueClamped(FVector2D(0.f, 650.f), FVector2D(MinSoundPitch, MaxSoundPitch), FGenericPlatformMath::Abs<float>(ForwardVelocity));
		ForwardSpeed = FMath::GetMappedRangeValueClamped(FVector2D(-700.f, 700.f), FVector2D(-1, 1), ForwardVelocity);

		FRotator NewYawRot = GetOwner()->GetActorRotation();
		float YawRotSpeed = FMath::FindDeltaAngleDegrees(NewYawRot.Yaw, LastYawRot.Yaw);
		LastYawRot = NewYawRot;
		TurningSpeed = FMath::GetMappedRangeValueClamped(FVector2D(-TurnRate, TurnRate), FVector2D(-1, 1), YawRotSpeed);
	}

	AnimateTracks(ForwardSpeed, TurningSpeed);
	TurnWheels(ForwardSpeed, TurnSpeed);
	
	TankSFXPitch(FGenericPlatformMath::Abs<float>(EnginePitch));
}


////////////////////////////////////////////////////////////////////////////////
// Animation data

void UTankMovement::TurnWheels(float ForwardSpeed, float TurnSpeed)
{
	float WheelSpeedYaw = 0.f;
	if (MoveForwardValue != 0.f)
	{
		WheelSpeedYaw = SetWheelTurnValue(ForwardSpeed);

		LeftFrontBackYaw -= WheelSpeedYaw;
		RightFrontBackYaw -= WheelSpeedYaw;
	}
	else if (TurnRightValue != 0.f)
	{
		WheelSpeedYaw = SetWheelTurnValue(TurnSpeed);

		LeftWheelYaw -= WheelSpeedYaw;
		LeftFrontBackYaw -= WheelSpeedYaw;

		RightWheelYaw += WheelSpeedYaw;
		RightFrontBackYaw += WheelSpeedYaw;
	}
}

float UTankMovement::SetWheelTurnValue(float TurnSpeed)
{
	float WheelSpeedYaw = TurnSpeed * WheelTurnMultiplier;

	// Don't let WheelTurnYaw get too high.
	if (LeftFrontBackYaw <= -36000.f || LeftFrontBackYaw >= 36000.f)
	{
		LeftFrontBackYaw = 0.f;
	}
	if (RightFrontBackYaw <= -36000.f || RightFrontBackYaw >= 36000.f)
	{
		RightFrontBackYaw = 0.f;
	}
	return WheelSpeedYaw;
}

void UTankMovement::AnimateTracks(float ForwardSpeed, float TurnSpeed)
{
	float TrackOffset = 0;

	if (MoveForwardValue != 0.f)
	{
		float MaxSpeed = ForwardSpeed * GetWorld()->GetDeltaSeconds();
		TrackOffset = MaxSpeed * TrackSpeedMultiplier;

		AnimateTrackMatLeft(-TrackOffset);
		AnimateTrackMatRight(-TrackOffset);
	}
	else if (TurnRightValue != 0.f)
	{
		TrackOffset = TurnSpeed * GetWorld()->GetDeltaSeconds();

		AnimateTrackMatLeft(TrackOffset);
		AnimateTrackMatRight(-TrackOffset);
	}
}

void UTankMovement::AnimateTrackMatLeft(float NewOffset)
{	
	if (LeftTrackMat != NULL)
	{
		float CurrentOffset;
		LeftTrackMat->GetScalarParameterValue(TrackScalarParamName, CurrentOffset);
		NewOffset += CurrentOffset;

		LeftTrackMat->SetScalarParameterValue(TrackScalarParamName, NewOffset);

		// Don't let Scalar value get too high.
		if (CurrentOffset < -10000.f || CurrentOffset > 10000.f)
		{
			LeftTrackMat->SetScalarParameterValue(TrackScalarParamName, 0.f);
		}
	}
}

void UTankMovement::AnimateTrackMatRight(float NewOffset)
{
	if (RightTrackMat != NULL)
	{
		float CurrentOffset;
		RightTrackMat->GetScalarParameterValue(TrackScalarParamName, CurrentOffset);
		NewOffset += CurrentOffset;

		RightTrackMat->SetScalarParameterValue(TrackScalarParamName, NewOffset);

		// Don't let Scalar value get too high.
		if (CurrentOffset < -10000.f || CurrentOffset > 10000.f)
		{
			RightTrackMat->SetScalarParameterValue(TrackScalarParamName, 0.f);
		}
	}
}

void UTankMovement::SetLeftTrackMat(UMaterialInstanceDynamic * TrackMat)
{
	if (TrackMat == NULL) { return; }
	LeftTrackMat = TrackMat;
}

void UTankMovement::SetRightTrackMat(UMaterialInstanceDynamic * TrackMat)
{
	if (TrackMat == NULL) { return; }
	RightTrackMat = TrackMat;
}


////////////////////////////////////////////////////////////////////////////////
// SFX

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

void UTankMovement::OnOwnerDeath()
{
	StopEngineSound();
}

