// Copyright 2018 - 2022 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplosionFX.generated.h"

class UParticleSystem;
class USoundBase;

UCLASS()
class UE4_BATTLETANK_API AExplosionFX : public AActor
{
	GENERATED_BODY()
	
public:
	///////////////////////////////////////////////////////////////////////////////////
	// VFX

	/** Impact fx on default */
	UPROPERTY(EditDefaultsOnly, Category = "ParticleEffects")
	UParticleSystem * DefaultFX;

	/** Impact fx on dirt */
	UPROPERTY(EditDefaultsOnly, Category = "ParticleEffects")
	UParticleSystem * GrassFX;

	/** Impact fx on dirt */
	UPROPERTY(EditDefaultsOnly, Category = "ParticleEffects")
	UParticleSystem * DirtFX;


	///////////////////////////////////////////////////////////////////////////////////
	// SFX

	/** Sound fx on default */
	UPROPERTY(EditDefaultsOnly, Category = "SoundEffects")
	USoundBase * DefaultSound;

	/** Sound fx on dirt */
	UPROPERTY(EditDefaultsOnly, Category = "SoundEffects")
	USoundBase * DirtSound;

	/** Sound fx on rock */
	UPROPERTY(EditDefaultsOnly, Category = "SoundEffects")
	USoundBase * RockSound;

	/** Sound fx on metal */
	UPROPERTY(EditDefaultsOnly, Category = "SoundEffects")
	USoundBase * MetalSound;


	////////////////////////////////////////////////////////////////
	// Get fx from surface

	/** SurfaceData for spawning */
	UPROPERTY(BlueprintReadOnly, Category = Surface)
	FHitResult SurfaceHit;

public:
	AExplosionFX();

	/** Spawn effect */
	virtual void PostInitializeComponents() override;

protected:
	/** Get fx for material type */
	UParticleSystem * GetExplosionFX(TEnumAsByte<EPhysicalSurface> SurfaceType) const;

	/** Get sound for material type */
	USoundBase * GetImpactSound(TEnumAsByte<EPhysicalSurface> SurfaceType) const;
	
};
