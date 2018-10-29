// Copyright 2018 Stuart McDonald.

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

	/** Damage dealt */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	float ProjectileDamage;

	/** Time till destroy */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	float DestroyDelay;

	/** Radius of damage */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	float DamageRadius;

	/** Damage type */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TSubclassOf<UDamageType> DamageType;

	/** Defaults */
	FProjectileData()
	{
		ProjectileDamage = 20.f;
		DestroyDelay = 10.f;
		DamageRadius = 1000.f;
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