// Copyright 2018 Stuart McDonald.

#include "Projectile.h"
#include "UE4_BattleTank.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/AudioComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "FX/ExplosionFX.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"


AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Collision Mesh"));
	SetRootComponent(CollisionMesh);
	CollisionMesh->SetNotifyRigidBodyCollision(true);
	CollisionMesh->SetVisibility(true);
	CollisionMesh->bReturnMaterialOnMove = true;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(FName("Projectile Movement"));
	ProjectileMovement->bAutoActivate = false;

	ProjectileSound = CreateDefaultSubobject<UAudioComponent>(FName("Audio FX"));
	ProjectileSound->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	ProjectileSound->bAutoActivate = true;

	ExplosionForce = CreateDefaultSubobject<URadialForceComponent>(FName("Explosion Force"));
	ExplosionForce->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	TrailFX = CreateDefaultSubobject<UParticleSystemComponent>(FName("TrailFX"));
	TrailFX->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	TrailFX->bAutoActivate = false;
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	CollisionMesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
}

void AProjectile::OnHit(UPrimitiveComponent * HitComponent, AActor * OtherActor, UPrimitiveComponent * OtherComponent, FVector NormalImpulse, const FHitResult & Hit)
{
	GetWorldTimerManager().ClearTimer(Timer);

	ProjectileSound->Deactivate();
	TrailFX->Deactivate();
	ProjectileMovement->Deactivate();

	SetRootComponent(TrailFX);
	CollisionMesh->DestroyComponent();
	ExplosionForce->FireImpulse();

	SpawnExplosionFX(Hit);

	UGameplayStatics::ApplyRadialDamage(
		this,
		ProjectileData.BaseDamage,
		GetActorLocation(),
		ProjectileData.DamageRadius,
		ProjectileData.DamageType,
		TArray<AActor*>(),
		this,
		Instigator->GetInstigatorController(),
		false
	);

	GetWorldTimerManager().SetTimer(Timer, this, &AProjectile::OnTimerExpire, ProjectileData.DestroyDelay, false);
}

void AProjectile::OnTimerExpire()
{
	Destroy();
}

void AProjectile::LaunchProjectile(float Speed)
{
	ProjectileMovement->SetVelocityInLocalSpace(FVector::ForwardVector * Speed);
	ProjectileMovement->Activate();
	TrailFX->Activate();
	GetWorldTimerManager().SetTimer(Timer, this, &AProjectile::OnTimerExpire, ProjectileData.DestroyDelay, false);
}

void AProjectile::SpawnExplosionFX(const FHitResult & Impact)
{
	if (ExplosionFX && Impact.bBlockingHit)
	{
		FHitResult UseImpact = Impact;

		FTransform const SpawnTransform(Impact.ImpactNormal.Rotation(), Impact.ImpactPoint);
		AExplosionFX * Effect = GetWorld()->SpawnActorDeferred<AExplosionFX>(ExplosionFX, SpawnTransform);

		if (Effect)
		{
			Effect->SurfaceHit = Impact;
			UGameplayStatics::FinishSpawningActor(Effect, SpawnTransform);
		}
	}
	else if (DefaultFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, DefaultFX, GetActorLocation());
	}
}

