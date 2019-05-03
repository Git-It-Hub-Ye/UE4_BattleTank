// Copyright 2018 Stuart McDonald.

#include "ExplosionFX.h"
#include "UE4_BattleTank.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AExplosionFX::AExplosionFX()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void AExplosionFX::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UPhysicalMaterial * HitPhysMat = SurfaceHit.PhysMaterial.Get();
	EPhysicalSurface HitSurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitPhysMat);

	/** Spawn impact fx */
	UParticleSystem * ExplosionFX = GetExplosionFX(HitSurfaceType);
	if (ExplosionFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionFX, GetActorLocation(), GetActorRotation());
	}

	/** Spawn impact sound */
	USoundBase * ExplosionSound = GetImpactSound(HitSurfaceType);
	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
	}

	// Destroy actor
	Destroy();
}

UParticleSystem * AExplosionFX::GetExplosionFX(TEnumAsByte<EPhysicalSurface> SurfaceType) const
{
	UParticleSystem * ExplosionFX = nullptr;

	switch (SurfaceType)
	{
	case SURFACE_GRASS:	ExplosionFX = GrassFX;	 break;
	case SURFACE_DIRT:	ExplosionFX = DirtFX;	 break;
	default:			ExplosionFX = DefaultFX; break;
	}

	return ExplosionFX;
}

USoundBase * AExplosionFX::GetImpactSound(TEnumAsByte<EPhysicalSurface> SurfaceType) const
{
	USoundBase * ExplosionSound = nullptr;

	switch (SurfaceType)
	{
	case SURFACE_METAL:            ExplosionSound = MetalSound;		 break;
	case SURFACE_ROCK:             ExplosionSound = RockSound;		 break;
	case SURFACE_DIRT:             ExplosionSound = DirtSound;		 break;
	case SURFACE_GRASS:            ExplosionSound = DirtSound;		 break;
	default:					   ExplosionSound = DefaultSound;	 break;
	}

	return ExplosionSound;
}

