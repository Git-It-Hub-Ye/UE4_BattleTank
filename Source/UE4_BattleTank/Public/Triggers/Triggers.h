// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Triggers.generated.h"

class UBoxComponent;
class USphereComponent;

UCLASS()
class UE4_BATTLETANK_API ATriggers : public AActor
{
	GENERATED_BODY()
	
protected:
	/** Volume to trigger behaviour when an actor overlaps */
	UPROPERTY(VisibleDefaultsOnly)
	UBoxComponent  * TriggerVolume;

public:
	ATriggers();

protected:
	virtual void BeginPlay() override;

	/** Checks for overlaps */
	UFUNCTION()
	virtual void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) PURE_VIRTUAL(ATriggers::OnOverlap, )
	
	/**  Checks for end overlaps  */
	UFUNCTION()
	virtual void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) PURE_VIRTUAL(ATriggers::OnEndOverlap, )

};
