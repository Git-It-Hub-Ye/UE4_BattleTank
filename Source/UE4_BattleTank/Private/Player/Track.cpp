// Copyright 2018 Stuart McDonald.

#include "Track.h"
#include "Engine/StaticMesh.h"
#include "Components/SkeletalMeshComponent.h"
#include "PhysicsEngine/ConstraintInstance.h"


UTrack::UTrack()
{
	PrimaryComponentTick.bCanEverTick = false;
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

	if (WheelBoneNames.Num() > 0 && RootBody->Constraints.Num() > 0)
	{
		for (int32 i = 0; i < RootBody->Constraints.Num(); i++)
		{
			for (int32 j = 0; j < WheelBoneNames.Num(); j++)
			{
				if (RootBody->Constraints[i]->JointName == WheelBoneNames[j])
				{
					FConstraintInstance * WheelCon = RootBody->Constraints[i];
					if (!WheelCon) { continue; }

					WheelConstraints.Add(WheelCon);
					WheelBones.Add(WheelCon->JointName);
				}
			}
		}
	}
}

void UTrack::SetThrottle(float Throttle, bool bIsTurning)
{
	float CurrentThrottle = FMath::Clamp<float>(Throttle, -1, 1);
	bTurningHalfSpeed = bIsTurning;

	if (CurrentThrottle == 0.f  && !bBrake) { ApplyBrakes(); }
	else if (CurrentThrottle != 0.f && bBrake) { bBrake = false; LimitWheelBoneConstraints(bBrake); }

	SetFrontAndRearWheelSpeed(CurrentThrottle);
	DriveWheels(CurrentThrottle);
}

void UTrack::DriveWheels(float CurrentThrottle)
{
	if (WheelBones.Num() <= 0) { return; }

	auto ForceApplied = CurrentThrottle * TrackMaxDrivingForce;
	auto ForcePerWheel = ForceApplied / WheelBones.Num();

	for (FName Wheel : WheelBones)
	{
		if (!RootBody) { return; }
		RootBody->AddForce(GetForwardVector() * ForcePerWheel, Wheel);
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

float UTrack::GetFrontAndRearWheelSpeed()
{
	return CurrentWheelSpeed;
}

