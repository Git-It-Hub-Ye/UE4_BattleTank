// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Turret.generated.h"

class USoundBase;
class UAudioComponent;

/**
* Responsible for rotating the turret.
*/
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UE4_BATTLETANK_API UTurret : public UStaticMeshComponent
{
	GENERATED_BODY()
	
private:
	/** Speed turret can rotate */
	UPROPERTY(EditAnywhere, Category = "Config", meta = (ClampMin = 1.f, ClampMax = 360.f))
	float MaxDegreesPerSecond = 25.0;

	/** Turret rotating sound loop */
	UPROPERTY(EditdefaultsOnly, Category = "Audio")
	USoundBase * RotateLoopSfx;

	/** Max pitch for turret sound, when rotating fast */
	UPROPERTY(EditAnywhere, Category = "Audio", meta = (ClampMin = 0.f, ClampMax = 2.f))
	float MaxSoundPitch = 2.f;

	/** Min pitch for turret sound, when rotating slow */
	UPROPERTY(EditAnywhere, Category = "Audio", meta = (ClampMin = 0.f, ClampMax = 2.f))
	float MinSoundPitch = 1.f;

	/** Is turret rotating */
	bool bIsRotating = false;

	/** Handles timer stoping sound */
	FTimerHandle SFXHandle;

	/** Component for start & end sounds */
	UAudioComponent * TurretAudio = nullptr;

public:
	/** Rotate turret */
	UFUNCTION(BlueprintCallable, Category = "Aim")
	void RotateTurret(float RelativeSpeed);

	/** Stop audio on turret */
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void StopTurretAudio();

private:
	/** Plays sound if turret is rotating */
	void TurretSFX(float RelativeSpeed);

	/** Set pitch of sound */
	void TurretSFXPitch(float RelativeSpeed);

	/** Start playing sfx */
	void SFXRotateTurretPlay();

	/** Stop playing sfx */
	void SFXRotateTurretStop();

	/** Play sound on turret */
	UAudioComponent * SFXPlay(USoundBase * SoundFX);
	
};
