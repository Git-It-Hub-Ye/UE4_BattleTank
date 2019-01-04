// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Barrel.generated.h"

class USoundBase;
class UAudioComponent;

/**
* Responsible for rotating the barrel.
*/
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UE4_BATTLETANK_API UBarrel : public UStaticMeshComponent
{
	GENERATED_BODY()
	
private:
	/** Speed barrel can elevate */
	UPROPERTY(EditDefaultsOnly, Category = "Config", meta = (ClampMin = 0.0f, ClampMax = 360.f))
	float MaxDegreesPerSecond = 10.0;

	/** Max degrees barrel can elevate */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	float MaxElevationDegrees = 40.;

	/** Min degrees barrel can elevate */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	float MinElevationDegrees = 0.;

	/** Barrel rotating sound loop */
	UPROPERTY(EditdefaultsOnly, Category = "Audio")
	USoundBase * RotateLoopSfx;

	/** Max pitch for barrel sound, when elevating fast */
	UPROPERTY(EditAnywhere, Category = "Audio", meta = (ClampMin = 0.f, ClampMax = 2.f))
	float MaxSoundPitch = 2.f;

	/** Min pitch for barrel sound, when elevating slow */
	UPROPERTY(EditAnywhere, Category = "Audio", meta = (ClampMin = 0.f, ClampMax = 2.f))
	float MinSoundPitch = 1.f;

	/** Is barrel rotating */
	bool bIsRotating = false;

	/** Handles timer stoping sound */
	FTimerHandle SFXHandle;

	/** Component for start & end sounds */
	UAudioComponent * BarrelAudio = nullptr;

public:
	/** Elevate barrel */
	UFUNCTION(BlueprintCallable, Category = "Aim")
	void ElevateBarrel(float RelativeSpeed);

private:
	/** Plays sound if barrel is rotating */
	void BarrelSFX(float RelativeSpeed);

	/** Set pitch of sound */
	void BarrelSFXPitch(float RelativeSpeed);

	/** Start playing sfx */
	void SFXElevateBarrelPlay();

	/** Stop playing sfx */
	void SFXElevateBarrelStop();

	/** Play sound on barrel */
	UAudioComponent * SFXPlay(USoundBase * SoundFX);
	
};
