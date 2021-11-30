// Copyright 2018 - 2021 Stuart McDonald.

#include "Projectile.h"
#include "UE4_BattleTank.h"
#include "Engine/World.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "FX/ExplosionFX.h"


AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Collision Mesh"));
	SetRootComponent(CollisionMesh);
	CollisionMesh->SetNotifyRigidBodyCollision(true);
	CollisionMesh->SetVisibility(true);
	CollisionMesh->bReturnMaterialOnMove = true;
	CollisionMesh->SetGenerateOverlapEvents(true);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(FName("Projectile Movement"));
	ProjectileMovement->bAutoActivate = false;

	FlybyAudioComp = CreateDefaultSubobject<UAudioComponent>(FName("Audio FX"));
	FlybyAudioComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	FlybyAudioComp->SetAutoActivate(false);

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
	CollisionMesh->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnOverlap);

	// Setup instigator controller, as instigator may be null if dead before projectile hits
	EventInstigator = GetInstigatorController();
}

void AProjectile::OnHit(UPrimitiveComponent * HitComponent, AActor * OtherActor, UPrimitiveComponent * OtherComponent, FVector NormalImpulse, const FHitResult & Hit)
{
	GetWorldTimerManager().ClearTimer(Timer);

	FlybyAudioComp->Stop();
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
		EventInstigator,
		false
	);

	GetWorldTimerManager().SetTimer(Timer, this, &AProjectile::OnTimerExpire, ProjectileData.DestroyDelay, false);
}

void AProjectile::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (FlybyAudioComp && OtherActor && GetOwner())
	{
		if (OtherActor != GetOwner())
		{
			FlybyAudioComp->Play();
		}
	}
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

