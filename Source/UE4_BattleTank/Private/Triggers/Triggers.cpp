// Copyright 2018 Stuart McDonald.

#include "Triggers.h"
#include "Engine/World.h"
#include "TimerManager.h"

#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"


ATriggers::ATriggers()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	TriggerVolume = CreateDefaultSubobject<UBoxComponent >(FName("Trigger Volume"));
	SetRootComponent(TriggerVolume);
	TriggerVolume->bGenerateOverlapEvents = true;
	TriggerVolume->bHiddenInGame = true;
}

void ATriggers::BeginPlay()
{
	Super::BeginPlay();
	if (TriggerVolume == nullptr) { return; }
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ATriggers::OnOverlap);
	TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &ATriggers::OnEndOverlap);
}

