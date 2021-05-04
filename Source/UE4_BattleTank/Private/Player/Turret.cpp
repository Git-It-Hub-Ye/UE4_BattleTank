// Copyright 2018 to 2021 Stuart McDonald.

#include "Turret.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

#include "Components/AudioComponent.h"
#include "Tank.h"


void UTurret::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner() != NULL)
	{
		ATank * OwnerPawn = Cast<ATank>(GetOwner());

		if (OwnerPawn == NULL) { return; }
		OwnerPawn->OnDeath.AddUniqueDynamic(this, &UTurret::OnOwnerDeath);
	}
}

void UTurret::OnOwnerDeath()
{
	StopTurretAudio();
}

void UTurret::RotateTurret(float RelativeSpeed)
{
	// Move Turret right amount this frame.
	// Given max Rotation speed and the frame rate.

	RelativeSpeed = FMath::Clamp<float>(RelativeSpeed, -1, +1);

	auto RotationChange = RelativeSpeed * MaxDegreesPerSecond * GetWorld()->DeltaTimeSeconds;
	auto NewRotation = RelativeRotation.Yaw + RotationChange;

	TurretSFX(RotationChange);
	TurretSFXPitch(FGenericPlatformMath::Abs<float>(RelativeSpeed));

	SetRelativeRotation(FRotator(0, NewRotation, 0));
}

void UTurret::TurretSFX(float RelativeSpeed)
{
	if (RelativeSpeed != 0.f && !bIsRotating)
	{
		bIsRotating = true;
		SFXRotateTurretPlay();
	}
	else if (RelativeSpeed == 0.f && bIsRotating)
	{
		// Stop playing sfx with timer for smoother sound
		bIsRotating = false;
		GetWorld()->GetTimerManager().SetTimer(SFXHandle, this, &UTurret::SFXRotateTurretStop, 0.2f, false);
	}
}

void UTurret::TurretSFXPitch(float RelativeSpeed)
{
	if (TurretAudio && RelativeSpeed != 0.f)
	{
		float NewPitch = FMath::GetMappedRangeValueClamped(FVector2D(0.f, 1.f), FVector2D(MinSoundPitch, MaxSoundPitch), RelativeSpeed);
		TurretAudio->SetPitchMultiplier(NewPitch);
	}
}

void UTurret::SFXRotateTurretPlay()
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	// Is active return
	if (TurretAudio && TurretAudio->IsActive()) { return; }

	TurretAudio = SFXPlay(RotateLoopSfx);
}

void UTurret::SFXRotateTurretStop()
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	if (TurretAudio)
	{
		TurretAudio->FadeOut(0.1f, 0.f);
		TurretAudio = nullptr;
	}
}

UAudioComponent * UTurret::SFXPlay(USoundBase * SoundFX)
{
	UAudioComponent * AC = nullptr;
	if (SoundFX)
	{
		AC = UGameplayStatics::SpawnSoundAttached(SoundFX, this);
		AC->FadeIn(0.1f, 1.f);
	}
	return AC;
}

void UTurret::StopTurretAudio()
{
	if (TurretAudio && TurretAudio->IsPlaying())
	{
		TurretAudio->Stop();
	}
}
