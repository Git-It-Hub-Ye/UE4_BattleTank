// Copyright 2018 Stuart McDonald.

#include "Triggers.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"


ATriggers::ATriggers()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(FName("Trigger Volume"));
	SetRootComponent(TriggerVolume);
}

void ATriggers::BeginPlay()
{
	Super::BeginPlay();
	if (TriggerVolume == nullptr) { return; }
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ATriggers::OnOverlap);
}

void ATriggers::OnOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	// No Default Behaviour
}

