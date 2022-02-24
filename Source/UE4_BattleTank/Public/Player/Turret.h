// Copyright 2018 - 2022 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Turret.generated.h"

class UAudioComponent;

/**
* Responsible for rotating the turret.
*/
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UE4_BATTLETANK_API UTurret : public UStaticMeshComponent
{
	GENERATED_BODY()
	
private:
	/** Speed turret can rotate */
	UPROPERTY(EditAnywhere, Category = "Config", meta = (ClampMin = 1.f, ClampMax = 360.f))
	float MaxDegreesPerSecond = 25.0;

	/** Time for turret audio to fade out */
	UPROPERTY(EditAnywhere, Category = "Sound")
	float FadeOutTime_SFX = 0.2;

	/** Component for start & end sounds */
	UPROPERTY(VisibleDefaultsOnly)
	UAudioComponent * TurretAudio = nullptr;

	bool bIsTurning;

public:
	UTurret();

	/** Rotate turret */
	UFUNCTION(BlueprintCallable, Category = "Aim")
	void RotateTurret(float RelativeSpeed);

protected:
	virtual void BeginPlay() override;

	/** Stop audio on turret */
	void StopTurretAudio();

private:

	UFUNCTION()
	void OnOwnerDeath();

	/** Plays sound if turret is rotating */
	void TurretSFX(float RelativeSpeed);
};
