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
	case SURFACE_METAL:	ExplosionFX = MetalFX;	 break;
	default:			ExplosionFX = DefaultFX; break;
	}

	return ExplosionFX;
}

