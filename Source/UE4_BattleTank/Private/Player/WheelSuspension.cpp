// Copyright 2018 Stuart McDonald.

#include "WheelSuspension.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"


AWheelSuspension::AWheelSuspension()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PostPhysics;

	MassAxleConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(FName("AxelConstraint"));
	SetRootComponent(MassAxleConstraint);

	Axle = CreateDefaultSubobject<USphereComponent>(FName("Axle"));
	Axle->SetupAttachment(RootComponent);

	Wheel = CreateDefaultSubobject<UStaticMeshComponent>(FName("Wheel"));
	Wheel->SetupAttachment(Axle);

	AxleWheelConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(FName("WheelConstraint"));
	AxleWheelConstraint->SetupAttachment(Axle);

	ForceToApplyThisFrame = 0.f;
}

void AWheelSuspension::BeginPlay()
{
	Super::BeginPlay();
	if (Wheel == nullptr) { return; }
	Wheel->SetNotifyRigidBodyCollision(true);
	Wheel->OnComponentHit.AddDynamic(this, &AWheelSuspension::OnHit);
	SetWheelConstraints();
}

void AWheelSuspension::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Reset force on wheel if not hitting surface (After physics has stopped)
	if (GetWorld()->TickGroup == TG_PostPhysics)
	{
		ForceToApplyThisFrame = 0.f;
	}
}

void AWheelSuspension::SetWheelConstraints()
{
	if (!GetAttachParentActor()) { return; }
	UPrimitiveComponent * BodyRoot = Cast<UPrimitiveComponent>(GetAttachParentActor()->GetRootComponent());
	if (!BodyRoot) { return; }
	MassAxleConstraint->SetConstrainedComponents(BodyRoot, NAME_None, Axle, NAME_None);
	AxleWheelConstraint->SetConstrainedComponents(Axle, NAME_None, Wheel, NAME_None);
}

void AWheelSuspension::OnHit(UPrimitiveComponent * HitComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{
	ApplyForce();
}

void AWheelSuspension::AddDrivingForce(float Force)
{
	ForceToApplyThisFrame += Force;
}

void AWheelSuspension::ApplyForce()
{
	Wheel->AddForce(Axle->GetForwardVector() * ForceToApplyThisFrame);
}

