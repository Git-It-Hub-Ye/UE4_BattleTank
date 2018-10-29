// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WheelSuspension.generated.h"

class UPhysicsConstraintComponent;
class USphereComponent;

/**
* Rolls along with a suspension, for softer collisions with surface
*/
UCLASS()
class UE4_BATTLETANK_API AWheelSuspension : public AActor
{
	GENERATED_BODY()
	
private:
	/** Apply constraints between Axle and component owner */
	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UPhysicsConstraintComponent * MassAxleConstraint = nullptr;

	/** Apply constraints between Axle and Wheel */
	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UPhysicsConstraintComponent * AxleWheelConstraint = nullptr;

	/** Attached to Owner and wheel & allows wheel to roll in the axis of the axle */
	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USphereComponent * Axle = nullptr;

	/** Rolls along surface */
	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UStaticMeshComponent * Wheel = nullptr;

	float ForceToApplyThisFrame;

public:
	/** Set default values and components */
	AWheelSuspension();

	virtual void Tick(float DeltaTime) override;

	/** Add force to wheel */
	void AddDrivingForce(float Force);

protected:
	virtual void BeginPlay() override;

private:
	/** Sets the constraints */
	void SetWheelConstraints();

	/** Adds force when wheel is on a surface */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Apply force to wheel */
	void ApplyForce();
	
};
