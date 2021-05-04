// Copyright 2018 to 2021 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UProjectileMovementComponent;
class URadialForceComponent;
class UAudioComponent;
class UParticleSystemComponent;
class UParticleSystem;
class AExplosionFX;

// Struct for projectile data
USTRUCT(BlueprintType)
struct FProjectileData {
	GENERATED_USTRUCT_BODY()

	/** Normal damage dealt */
	UPROPERTY(EditDefaultsOnly, Category = "Config", meta = (ClampMin = 1.f, ClampMax = 100.f))
	float BaseDamage;

	/** Time till destroy */
	UPROPERTY(EditDefaultsOnly, Category = "Config", meta = (ClampMin = 1.f, ClampMax = 10.f))
	float DestroyDelay;

	/** Radius of damage, actors within this are will recieve damage */
	UPROPERTY(EditDefaultsOnly, Category = "Config", meta = (ClampMin = 100.f, ClampMax = 2000.f))
	float DamageRadius;

	/** Radius of effect, actors within this radius will recieve effects from projectile ie.(Camera shake) */
	UPROPERTY(EditDefaultsOnly, Category = "Config", meta = (ClampMin = 100.f, ClampMax = 2000.f))
	float EffectRadius;

	/** Damage type */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TSubclassOf<UDamageType> DamageType;

	/** Defaults */
	FProjectileData()
	{
		BaseDamage = 10.f;
		DestroyDelay = 10.f;
		DamageRadius = 1000.f;
		EffectRadius = 1500.f;
		DamageType = UDamageType::StaticClass();
	}
};

UCLASS()
class UE4_BATTLETANK_API AProjectile : public AActor
{
	GENERATED_BODY()
	
private:
	/** Projectile mesh */
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	UStaticMeshComponent * CollisionMesh = nullptr;

	/** Controls projectile movement */
	UPROPERTY(VisibleDefaultsOnly, Category = "Movement")
	UProjectileMovementComponent * ProjectileMovement = nullptr;

	/** Force added to hit actors */
	UPROPERTY(VisibleAnywhere, Category = "Components")
	URadialForceComponent * ExplosionForce = nullptr;

	/** FX component for trail */
	UPROPERTY(VisibleAnywhere, Category = "FX")
	UParticleSystemComponent * TrailFX = nullptr;

	/** Projectile movement sound fx */
	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UAudioComponent * ProjectileSound;

	/** FX for surface type */
	UPROPERTY(EditDefaultsOnly, Category = "FX")
	TSubclassOf<AExplosionFX> ExplosionFX;

	/** Default explosion fx */
	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UParticleSystem * DefaultFX;

	/** Data for projectile */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	FProjectileData ProjectileData;

	/** Controller that spawned projectile */
	UPROPERTY()
	AController * EventInstigator;

	/** Timer handle for destroy */
	FTimerHandle Timer;

public:
	AProjectile();

	/** Launch projectile forwards */
	void LaunchProjectile(float Speed);

protected:
	virtual void BeginPlay() override;

private:
	/** Deals damage within radius and plays fx on hit */
	UFUNCTION()
	void OnHit(UPrimitiveComponent * HitComponent, AActor * OtherActor, UPrimitiveComponent * OtherComponent, FVector NormalImpulse, const FHitResult & Hit);

	/** Destroys actor */
	void OnTimerExpire();

	/** Play explosion fx */
	void SpawnExplosionFX(const FHitResult & Impact);

};
