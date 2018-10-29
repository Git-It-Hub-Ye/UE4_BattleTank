// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Turret.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UE4_BATTLETANK_API UTurret : public UStaticMeshComponent
{
	GENERATED_BODY()
	
private:
	/** Speed turret can rotate */
	UPROPERTY(EditAnywhere, Category = Setup)
	float MaxDegreesPerSecond = 25.0;

public:
	/** Rotate turret */
	UFUNCTION(BlueprintCallable, Category = "Aim")
	void RotateTurret(float RelativeSpeed);
	
};
