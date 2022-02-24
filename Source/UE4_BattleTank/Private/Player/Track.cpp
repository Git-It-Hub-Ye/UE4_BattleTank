// Copyright 2018 - 2022 Stuart McDonald.

#include "Track.h"
#include "Engine/StaticMesh.h"
#include "PhysicsEngine/ConstraintInstance.h"

#include "Components/SkeletalMeshComponent.h"
#include "WheelComponent.h"


UTrack::UTrack()
{
	PrimaryComponentTick.bCanEverTick = false;
	bBrake = true;
}

void UTrack::BeginPlay()
{
	Super::BeginPlay();
	GetWheels();
}

void UTrack::GetWheels()
{
	if (!GetOwner()) { return; }
	RootBody = Cast<USkeletalMeshComponent>(GetOwner()->GetRootComponent());

	if (!RootBody) { return; }
	if (WheelBoneNames.Num() <= 0 && RootBody->Constraints.Num() <= 0) { return; }

	// Get all physics constraints attached to wheel bones
	for (int32 i = 0; i < RootBody->Constraints.Num(); i++)
	{
		for (int32 j = 0; j < WheelBoneNames.Num(); j++)
		{
			if (RootBody->Constraints[i]->JointName != WheelBoneNames[j]) { continue; }
				
			FConstraintInstance * WheelCon = RootBody->Constraints[i];
			if (!WheelCon) { continue; }

			WheelConstraints.Add(WheelCon);
			WheelCon->SetAngularSwing2Motion(EAngularConstraintMotion::ACM_Locked);
		}
	}

	if (WheelConstraints.Num() <= 0) { return; }

	// Get all wheel components attached to track, will add force to attached wheel
	// If no wheel components, can't add force to wheels.
	TArray<USceneComponent*> Children;
	GetChildrenComponents(true, Children);

	for (int32 i = 0; i < Children.Num(); i++)
	{
		UWheelComponent * WheelComp = Cast<UWheelComponent>(Children[i]);
		if (!WheelComp) { continue; }
	
		WheelComp->AttachToComponent(RootBody, FAttachmentTransformRules::SnapToTargetNotIncludingScale, WheelConstraints[i]->JointName);
		WheelComponents.Add(WheelComp);
		if (WheelConstraints.Num() - 1 == i) { return; }
	}
}

void UTrack::SetThrottle(float Throttle, bool bIsTurning)
{
	float CurrentThrottle = FMath::Clamp<float>(Throttle, -1, 1);
	
	bTurningHalfSpeed = bIsTurning;
	SetFrontAndRearWheelSpeed(CurrentThrottle);

	if (CurrentThrottle == 0.f && !bBrake) { ApplyBrakes(); }
	else if (CurrentThrottle != 0.f && bBrake) { bBrake = false; LimitWheelBoneConstraints(bBrake); }

	DriveWheels(CurrentThrottle);
}

void UTrack::DriveWheels(float CurrentThrottle)
{
	if (WheelComponents.Num() <= 0) { return; }

	auto ForceApplied = CurrentThrottle * TrackMaxDrivingForce;
	auto ForcePerWheel = ForceApplied / WheelComponents.Num();

	for (UWheelComponent * Wheel : WheelComponents)
	{
		if (!Wheel) { continue; }
		Wheel->DriveWheel(ForcePerWheel);
	}
}

void UTrack::ApplyBrakes()
{
	if (bBrake) { return; }
	bBrake = true;
	LimitWheelBoneConstraints(bBrake);
}

void UTrack::LimitWheelBoneConstraints(bool bBraking)
{
	if (WheelConstraints.Num() <= 0) { return; }

	for (FConstraintInstance * Constraint : WheelConstraints)
	{
		if (!Constraint) { continue; }
		if (bBrake)
		{
			Constraint->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Limited, 170.f);
		}
		else
		{
			Constraint->SetAngularSwing2Motion(EAngularConstraintMotion::ACM_Free);
		}
	}
}

void UTrack::SetFrontAndRearWheelSpeed(float Throttle)
{
	if (bTurningHalfSpeed) { Throttle /= 2; }
	float InitialSpeed = FMath::Lerp<float>(0, FrontAndRearWheelSpeed, Throttle);
	CurrentWheelSpeed = FMath::Clamp<float>(InitialSpeed, -FrontAndRearWheelSpeed, FrontAndRearWheelSpeed);
}

