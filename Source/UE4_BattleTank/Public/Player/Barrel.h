// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Barrel.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UE4_BATTLETANK_API UBarrel : public UStaticMeshComponent
{
	GENERATED_BODY()
	
private:
	/** Speed barrel can elevate */
	UPROPERTY(EditDefaultsOnly, Category = Setup)
	float MaxDegreesPerSecond = 10.0;

	/** Max degrees barrel can elevate */
	UPROPERTY(EditDefaultsOnly, Category = Setup)
	float MaxElevationDegrees = 40.;

	/** Min degrees barrel can elevate */
	UPROPERTY(EditDefaultsOnly, Category = Setup)
	float MinElevationDegrees = 0.;

public:
	/** Elevate barrel */
	UFUNCTION(BlueprintCallable, Category = "Aim")
	void ElevateBarrel(float RelativeSpeed);
	
};
