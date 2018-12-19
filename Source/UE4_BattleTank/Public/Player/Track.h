// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Track.generated.h"

/**
* Track is used to set maximum driving force and apply force to the wheels
*/

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UE4_BATTLETANK_API UTrack : public UStaticMeshComponent
{
	GENERATED_BODY()
	
private:
	/** Max force to add to track */
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float TrackMaxDrivingForce = 400000;

	/** How many actros are tracks overlapping */
	int32 OverlappedActors = 0;

public:
	UTrack();

	/** Set how much force to add to track */
	void SetThrottle(float Throttle);

protected:
	virtual void BeginPlay() override;

private:
	/** How many ovelapping actors */
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	/** How many ovelapping actors */
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
