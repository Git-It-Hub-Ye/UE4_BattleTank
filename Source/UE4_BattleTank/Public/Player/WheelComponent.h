// Copyright 2018 to 2021 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "WheelComponent.generated.h"

/**
 * Attached to wheels, checking if wheel is on a drivable surface before applying force to wheel.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UE4_BATTLETANK_API UWheelComponent : public USphereComponent
{
	GENERATED_BODY()

private:
	/** Skeletal root, for adding force to bone */
	USkeletalMeshComponent * RootBody;

	/** Number of drivable surfaces wheel is touching, (if 0 add no force) */
	int32 OverlappedSurfaces = 0;
	
public:
	/** Add force to wheel if touching drivable surface */
	void DriveWheel(float ForceToApply);
	
protected:
	virtual void BeginPlay() override;

private:
	/** Add to ovelapped surface count when touching a new surface */
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	/** Remove from ovelapped surface count when leaving a surface */
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
};
