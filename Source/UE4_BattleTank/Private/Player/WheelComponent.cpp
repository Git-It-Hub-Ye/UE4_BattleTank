// Copyright 2018 - 2021 Stuart McDonald.

#include "WheelComponent.h"

#include "Components/SkeletalMeshComponent.h"


void UWheelComponent::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwner())
	{
		RootBody = Cast<USkeletalMeshComponent>(GetOwner()->GetRootComponent());
	}

	OnComponentBeginOverlap.AddDynamic(this, &UWheelComponent::OnOverlap);
	OnComponentEndOverlap.AddDynamic(this, &UWheelComponent::OnEndOverlap);
}

void UWheelComponent::OnOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor == GetOwner()) { return; }
	OverlappedSurfaces++;
}

void UWheelComponent::OnEndOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == GetOwner() || OverlappedSurfaces <= 0) { return; }
	OverlappedSurfaces--;
}

void UWheelComponent::DriveWheel(float ForceToApply)
{
	if (!RootBody || OverlappedSurfaces <= 0) { return; }
	RootBody->AddForce(RootBody->GetForwardVector() * ForceToApply, GetAttachSocketName());
}

