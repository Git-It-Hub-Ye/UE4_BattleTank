// Copyright 2018 Stuart McDonald.

#include "Triggers.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Player/Tank.h"
#include "AimingComponent.h"


ATriggers::ATriggers()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Static Mesh"));
	SetRootComponent(VisualMesh);

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(FName("Trigger Volume"));
	TriggerVolume->SetupAttachment(RootComponent);
}

void ATriggers::BeginPlay()
{
	Super::BeginPlay();
	if (TriggerVolume == nullptr) { return; }
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ATriggers::OnOverlap);
}

void ATriggers::OnOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	ATank * Tank = Cast<ATank>(OtherActor);

	if (Tank && Tank->IsPlayerControlled())
	{
		TriggerdBehaviour(Tank);
	}

	return;
}

void ATriggers::TriggerdBehaviour(ATank * Tank)
{
	// No Default Behaviour
}

