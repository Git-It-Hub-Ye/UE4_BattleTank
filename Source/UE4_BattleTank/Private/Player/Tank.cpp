// Copyright 2018 Stuart McDonald.

#include "Tank.h"
#include "TankPlayerController.h"
#include "Online/BattleTankGameModeBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "AimingComponent.h"
#include "UI/BattleHUD.h"
#include "TankMovement.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/PhysicalMaterial.h"


ATank::ATank()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	TankBody = CreateDefaultSubobject<USkeletalMeshComponent>(FName("TankBody"));
	SetRootComponent(TankBody);
	// Will set physics after spawn in begin play to avoid teleport flag errors.
	TankBody->SetSimulatePhysics(false);
	
	AimingComp = CreateDefaultSubobject<UAimingComponent>(FName("AimComponent"));
	MovementComp = CreateDefaultSubobject<UTankMovement>(FName("MoveComponent"));

	StartingHealth = 100;
	StartingArmour = 50;
	DestroyTimer = 5.f;
	bHasBeenDestroyed = false;
}

void ATank::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = StartingHealth;
	CurrentArmour = StartingArmour;
	TankBody->SetSimulatePhysics(true);
	UpdatePlayerHud();
}

void ATank::SetupPlayerInputComponent(UInputComponent * PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAction("Fire", IE_Pressed, this, &ATank::Fire);
}

void ATank::Fire()
{
	if (AimingComp && !IsTankDestroyed())
	{
		AimingComp->FireProjectile();
	}
}

float ATank::TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser)
{
	DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	int32 DamagePoints = FPlatformMath::RoundToInt(DamageAmount);
	int32 DamageToApply = FMath::Clamp(DamagePoints, 0, CurrentHealth);

	if (!IsTankDestroyed())
	{
		int32 DamageDealt = DamageToApply;
		if (CurrentArmour > 0)
		{
			int32 InitalArmour = CurrentArmour;
			CurrentArmour -= DamageDealt;
			CurrentArmour = FMath::Clamp<int32>(CurrentArmour, 0, InitalArmour);

			int32 ReducedDamage = InitalArmour - CurrentArmour;
			DamageDealt -= ReducedDamage;
		}

		if (DamageDealt > 0)
		{
			CurrentHealth -= DamageDealt;
		}

		UpdatePlayerHud();

		ATankPlayerController * const PC = this->Controller ? Cast<ATankPlayerController>(this->Controller) : nullptr;
		if (PC)
		{
			PC->ClientPlayCameraShake(DamageCamShakeBP);
		}

		if (CurrentHealth <= 0)
		{
			bHasBeenDestroyed = true;
			OnDeathBehaviour(EventInstigator);
		}
	}
	return DamageToApply;
}	

void ATank::OnDeathBehaviour(AController * EventInstigator)
{
	if (DestroyedFX)
	{
		ParticleComp = UGameplayStatics::SpawnEmitterAtLocation(this, DestroyedFX, GetActorLocation());
	}

	TankBody->SetCollisionProfileName("DestroyedTank");

	ABattleTankGameModeBase * const GM = GetWorld()->GetAuthGameMode<ABattleTankGameModeBase>();
	if (GM && Controller)
	{
		GM->HandleKill(Controller, EventInstigator);
	}

	ATankPlayerController * const PC = this->Controller ? Cast<ATankPlayerController>(this->Controller) : nullptr;
	if (PC && EventInstigator && EventInstigator->GetPawn())
	{
		PC->EnemyThatKilledPlayer(EventInstigator->GetPawn()->GetActorLocation());
	}

	OnTankDestroyed(bHasBeenDestroyed);
	OnDeath.Broadcast();
	SetLifeSpan(DestroyTimer);
}

void ATank::UpdatePlayerHud()
{
	ATankPlayerController * PC = Cast<ATankPlayerController>(GetController());
	ABattleHUD * BHUD = PC ? PC->GetPlayerHud() : nullptr;
	if (BHUD)
	{
		BHUD->UpdateHealthDisplay();
	}
}

void ATank::ReplenishHealth(float HealthToAdd)
{
	CurrentHealth = CurrentHealth + HealthToAdd;
	CurrentHealth = FMath::Clamp<int32>(CurrentHealth, 0, StartingHealth);
	UpdatePlayerHud();
}

void ATank::ReplenishArmour()
{
	CurrentArmour = StartingArmour;
	UpdatePlayerHud();
}

