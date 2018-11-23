// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Triggers.generated.h"

class UBoxComponent;

UCLASS()
class UE4_BATTLETANK_API ATriggers : public AActor
{
	GENERATED_BODY()
	
protected:
	/** Volume to trigger behaviour when an actor to overlaps */
	UPROPERTY(VisibleDefaultsOnly)
	UBoxComponent * TriggerVolume;

public:
	// Sets default values for this actor's properties
	ATriggers();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
};
