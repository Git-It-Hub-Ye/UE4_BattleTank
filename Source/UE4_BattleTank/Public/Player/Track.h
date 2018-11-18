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
	UPROPERTY(EditDefaultsOnly, Category = Setup)
	float TrackMaxDrivingForce = 400000;

public:
	UTrack();

	void SetThrottle(float Throttle);

protected:
	virtual void BeginPlay() override;

};
