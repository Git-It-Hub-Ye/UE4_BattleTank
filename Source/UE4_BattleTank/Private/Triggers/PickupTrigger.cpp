// Copyright 2018 to 2021 Stuart McDonald.

#include "PickupTrigger.h"
#include "Engine/World.h"
#include "TimerManager.h"

#include "Components/SphereComponent.h"
#include "BattleTankGameModeBase.h"


APickupTrigger::APickupTrigger()
{
	TriggerVolume = CreateDefaultSubobject<USphereComponent >(FName("Trigger Volume"));
	SetRootComponent(TriggerVolume);
	TriggerVolume->bGenerateOverlapEvents = true;
	TriggerVolume->bHiddenInGame = true;

	ArmourVolume = CreateDefaultSubobject<USphereComponent >(FName("Armour Volume"));
	ArmourVolume->SetupAttachment(RootComponent);
	ArmourVolume->bGenerateOverlapEvents = false;
	ArmourVolume->bHiddenInGame = false;
	ArmourVolume->bApplyImpulseOnDamage = false;

	AmountToGive = 10.f;
	ReapplyPickupTime = 1.f;
	StartingArmour = 10.f;

	bApplyToAllPlayers = false;
}

void APickupTrigger::BeginPlay()
{
	Super::BeginPlay();
	CurrentArmour = StartingArmour;

	if (TriggerVolume == nullptr) { return; }
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &APickupTrigger::OnOverlap);
	TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &APickupTrigger::OnEndOverlap);
}

float APickupTrigger::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	int32 DamagePoints = FPlatformMath::RoundToInt(DamageAmount);
	int32 DamageToApply = FMath::Clamp(DamagePoints, 0, CurrentArmour);

	CurrentArmour -= DamageToApply;
	if (CurrentArmour <= 0)
	{
		GetWorldTimerManager().ClearAllTimersForObject(this);

		if (ArmourVolume) { ArmourVolume->DestroyComponent(); }
		TriggerVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		TriggerVolume->Deactivate();
		HasBeenDestroyed();
	}
	return DamageToApply;
}

void APickupTrigger::OnEndOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("YES"))
	GetWorldTimerManager().ClearTimer(Handle_ApplyPickup);
}

void APickupTrigger::HasBeenDestroyed()
{
	GetWorldTimerManager().ClearAllTimersForObject(this);
	ABattleTankGameModeBase * BTGM = Cast<ABattleTankGameModeBase>(GetWorld()->GetAuthGameMode());
	if (BTGM)
	{
		BTGM->TriggerDestroyed();
	}
	Destroy();
}

