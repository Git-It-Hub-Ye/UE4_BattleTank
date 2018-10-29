// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Triggers.generated.h"

class UBoxComponent;
class ATank;

UCLASS()
class UE4_BATTLETANK_API ATriggers : public AActor
{
	GENERATED_BODY()
	
private:
	/** Volume to trigger behaviour when an actor to overlaps */
	UPROPERTY(VisibleDefaultsOnly)
	UBoxComponent * TriggerVolume;

	/** Visual mesh */
	UPROPERTY(VisibleDefaultsOnly)
	UStaticMeshComponent * VisualMesh;

public:
	// Sets default values for this actor's properties
	ATriggers();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	virtual void TriggerdBehaviour(ATank * Tank);
	
};
