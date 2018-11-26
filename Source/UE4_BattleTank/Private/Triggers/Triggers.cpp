// Copyright 2018 Stuart McDonald.

#include "Triggers.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "BattleTankGameModeBase.h"
#include "Engine/World.h"
#include "TimerManager.h"


ATriggers::ATriggers()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(FName("Trigger Volume"));
	SetRootComponent(TriggerVolume);
	TriggerVolume->bGenerateOverlapEvents = true;
	TriggerVolume->bHiddenInGame = true;
	TriggerVolume->bApplyImpulseOnDamage = false;

	ArmourVolume = CreateDefaultSubobject<UBoxComponent>(FName("Armour Volume"));
	ArmourVolume->SetupAttachment(RootComponent);
	ArmourVolume->bGenerateOverlapEvents = false;
	ArmourVolume->bHiddenInGame = false;
	ArmourVolume->bApplyImpulseOnDamage = false;

	StartingArmour = 10.f;
	CurrentArmour = 0.f;
}

void ATriggers::BeginPlay()
{
	Super::BeginPlay();
	CurrentArmour = StartingArmour;
	if (TriggerVolume == nullptr) { return; }
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ATriggers::OnOverlap);
}

void ATriggers::OnOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	// No Default Behaviour
}

float ATriggers::TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser)
{
	int32 DamagePoints = FPlatformMath::RoundToInt(DamageAmount);
	int32 DamageToApply = FMath::Clamp(DamagePoints, 0, CurrentArmour);

	CurrentArmour -= DamageToApply;
	UE_LOG(LogTemp, Warning, TEXT("%d"), CurrentArmour)

	if (CurrentArmour <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Dead"))
		GetWorldTimerManager().ClearAllTimersForObject(this);
		ArmourVolume->DestroyComponent();
		TriggerVolume->Deactivate();
		GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &ATriggers::HasBeenDestroyed, 3.f, false);
	}
	return DamageToApply;
}

void ATriggers::HasBeenDestroyed()
{
	GetWorldTimerManager().ClearAllTimersForObject(this);
	ABattleTankGameModeBase * BTGM = Cast<ABattleTankGameModeBase>(GetWorld()->GetAuthGameMode());
	if (BTGM)
	{
		BTGM->TriggerDestroyed();
	}
	Destroy();
}

