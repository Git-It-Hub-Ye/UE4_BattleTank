// Copyright 2018 Stuart McDonald.

#include "Barrel.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "Tank.h"


void UBarrel::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner() != NULL)
	{
		ATank * OwnerPawn = Cast<ATank>(GetOwner());

		if (OwnerPawn == NULL) { return; }
		OwnerPawn->OnDeath.AddUniqueDynamic(this, &UBarrel::OnOwnerDeath);
	}
}

void UBarrel::OnOwnerDeath()
{
	StopBarrelAudio();
}

void UBarrel::ElevateBarrel(float RelativeSpeed)
{
	// Move Barrel right amount this frame.
	// Given max elevation speed and the frame rate.

	RelativeSpeed = FMath::Clamp<float>(RelativeSpeed, -1, +1);

	auto ElevationChange = RelativeSpeed * MaxDegreesPerSecond * GetWorld()->DeltaTimeSeconds;
	auto NewRawElevation = RelativeRotation.Pitch + ElevationChange;
	auto Elevation = FMath::Clamp<float>(NewRawElevation, MinElevationDegrees, MaxElevationDegrees);

	BarrelSFX(ElevationChange);
	BarrelSFXPitch(FGenericPlatformMath::Abs<float>(RelativeSpeed));

	SetRelativeRotation(FRotator(Elevation, 0, 0));
}

void UBarrel::BarrelSFX(float RelativeSpeed)
{
	if (RelativeSpeed != 0.f && !bIsRotating)
	{
		bIsRotating = true;
		SFXElevateBarrelPlay();
	}
	else if (RelativeSpeed == 0.f && bIsRotating)
	{
		// Stop playing sfx with timer for smoother sound
		bIsRotating = false;
		GetWorld()->GetTimerManager().SetTimer(SFXHandle, this, &UBarrel::SFXElevateBarrelStop, 0.2f, false);
	}
}

void UBarrel::BarrelSFXPitch(float RelativeSpeed)
{
	if (BarrelAudio && RelativeSpeed != 0.f)
	{
		float NewPitch = FMath::GetMappedRangeValueClamped(FVector2D(0.f, 1.f), FVector2D(MinSoundPitch, MaxSoundPitch), RelativeSpeed);
		BarrelAudio->SetPitchMultiplier(NewPitch);
	}
}

void UBarrel::SFXElevateBarrelPlay()
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	// Is active return
	if (BarrelAudio && BarrelAudio->IsActive()) { return; }

	BarrelAudio = SFXPlay(RotateLoopSfx);
}

void UBarrel::SFXElevateBarrelStop()
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	if (BarrelAudio)
	{
		BarrelAudio->FadeOut(0.1f, 0.f);
		BarrelAudio = nullptr;
	}
}

UAudioComponent * UBarrel::SFXPlay(USoundBase * SoundFX)
{
	UAudioComponent * AC = nullptr;
	if (SoundFX)
	{
		AC = UGameplayStatics::SpawnSoundAttached(SoundFX, this);
		AC->FadeIn(0.1f, 1.f);
	}
	return AC;
}

void UBarrel::StopBarrelAudio()
{
	if (BarrelAudio && BarrelAudio->IsPlaying())
	{
		BarrelAudio->Stop();
	}
}

