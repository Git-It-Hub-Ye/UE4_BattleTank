// Copyright 2018 - 2021 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Barrel.generated.h"

/**
* Responsible for rotating the barrel.
*/
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UE4_BATTLETANK_API UBarrel : public UStaticMeshComponent
{
	GENERATED_BODY()
	
private:
	/** Speed barrel can elevate */
	UPROPERTY(EditDefaultsOnly, Category = "Config", meta = (ClampMin = 0.0f, ClampMax = 360.f))
	float MaxDegreesPerSecond = 10.0;

	/** Max degrees barrel can elevate */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	float MaxElevationDegrees = 40.;

	/** Min degrees barrel can elevate */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	float MinElevationDegrees = 0.;

public:
	/** Elevate barrel */
	UFUNCTION(BlueprintCallable, Category = "Aim")
	void ElevateBarrel(float RelativeSpeed);

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnOwnerDeath();
	
};
