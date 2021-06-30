// Copyright 2018 - 2021 Stuart McDonald.

#include "Turret.h"
#include "Engine/World.h"

#include "Components/AudioComponent.h"
#include "Tank.h"
#include "TankVehicle.h"


UTurret::UTurret()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryComponentTick.bCanEverTick = false;

	TurretAudio = CreateDefaultSubobject<UAudioComponent>(FName("TurretAudio"));
	TurretAudio->SetupAttachment(this);
	TurretAudio->SetAutoActivate(false);

	bIsTurning = false;
}

void UTurret::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner() != NULL)
	{
		ATank * TankOwner = Cast<ATank>(GetOwner());

		if (TankOwner == NULL) { return; }
		TankOwner->OnDeath.AddUniqueDynamic(this, &UTurret::OnOwnerDeath);
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

	TurretSFX(RelativeSpeed);
	SetRelativeRotation(FRotator(0, NewRotation, 0));
}

void UTurret::TurretSFX(float RelativeSpeed)
{
	if (TurretAudio)
	{
		if (RelativeSpeed != 0.f)
		{
			if (!bIsTurning && FMath::Abs(RelativeSpeed) >= 0.3f)
			{
				bIsTurning = true;
				TurretAudio->Play();
			}
			else
			{
				TurretAudio->SetFloatParameter("Speed", RelativeSpeed);
			}
		}
		else if (bIsTurning && TurretAudio->IsPlaying())
		{
			bIsTurning = false;
			TurretAudio->FadeOut(FadeOutTime_SFX, 0.f);
		}
	}
}

void UTurret::StopTurretAudio()
{
	if (TurretAudio && TurretAudio->IsPlaying())
	{
		TurretAudio->Stop();
	}
}
