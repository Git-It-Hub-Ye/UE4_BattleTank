// Copyright 2018 Stuart McDonald.

#include "Tank.h"
#include "TankPlayerController.h"
#include "Components/SkeletalMeshComponent.h"
#include "AimingComponent.h"
#include "TankMovement.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

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
	MaxArmour = 50;
	DestroyTimer = 5.f;
	bHasBeenDestroyed = false;
}

void ATank::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = StartingHealth;
	CurrentArmour = MaxArmour;
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
	if (AimingComp && CanRecieveInput())
	{
		AimingComp->FireProjectile();
	}
}

void ATank::ApplyHandbrake(bool bBrake)
{
	if (WheelBodies.Num() != 0 && TankBody->Bodies.Num() != 0)
	{
		for (int32 i = 0; i < WheelBodies.Num(); i++)
		{
			FBodyInstance * Body = TankBody->GetBodyInstance(WheelBodies[i], false);
			if (Body)
			{
				if (bBrake)
				{
					Body->bLockXRotation = true;
					Body->bLockYRotation = true;
					Body->SetDOFLock(EDOFMode::SixDOF);
				}
				else
				{
					Body->bLockXRotation = false;
					Body->bLockYRotation = true;
					Body->SetDOFLock(EDOFMode::None);
				}
			}
		}
	}
}

float ATank::TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser)
{
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

		if (CurrentHealth <= 0)
		{
			if (DestroyedFX)
			{
				ParticleComp = UGameplayStatics::SpawnEmitterAtLocation(this, DestroyedFX, GetActorLocation());
			}
			TankBody->SetCollisionProfileName("DestroyedTank");
			bHasBeenDestroyed = true;
			OnDeath.Broadcast();
			GetWorldTimerManager().SetTimer(DestroyHandle, this, &ATank::DestroyTank, DestroyTimer, false);
		}

		ATankPlayerController * const PC = this->Controller ? Cast<ATankPlayerController>(this->Controller) : nullptr;
		if (PC)
		{
			PC->ClientPlayCameraShake(DamageCamShakeBP);
		}
	}
	return DamageToApply;
}	

void ATank::DestroyTank()
{
	Destroy();
}

void ATank::UpdatePlayerHud()
{
	ATankPlayerController * PC = Cast<ATankPlayerController>(GetController());
	if (PC)
	{
		PC->UpdateHealthDisplay();
	}
}

bool ATank::CanRecieveInput()
{
	ATankPlayerController * PC = Cast<ATankPlayerController>(GetController());
	if (PC)
	{
		return PC->CanRecieveInput();
	}

	return true;
}

float ATank::GetHealthPercent() const
{
	return (float)CurrentHealth / (float)StartingHealth;
}

void ATank::ReplenishHealth(float HealthToAdd)
{
	CurrentHealth = CurrentHealth + HealthToAdd;
	CurrentHealth = FMath::Clamp<int32>(CurrentHealth, 0, StartingHealth);
	UpdatePlayerHud();
}

void ATank::ReplenishArmour()
{
	CurrentArmour = MaxArmour;
	UpdatePlayerHud();
}

