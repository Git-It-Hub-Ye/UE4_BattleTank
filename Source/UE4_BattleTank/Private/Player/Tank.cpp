// Copyright 2018 Stuart McDonald.

#include "Tank.h"
#include "UE4_BattleTank.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AudioComponent.h"

#include "Online/BattleTankGameModeBase.h"
#include "TankPlayerController.h"
#include "TankMovement.h"
#include "AimingComponent.h"
#include "UI/BattleHUD.h"


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
	UpdatePlayerHud();

	if (!TankBody) { return; }
	TankBody->SetSimulatePhysics(true);
	TankBody->OnComponentHit.AddDynamic(this, &ATank::OnHit);
}


////////////////////////////////////////////////////////////////////////////////////
// Player Input

void ATank::SetupPlayerInputComponent(UInputComponent * PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAction("Fire", IE_Pressed, this, &ATank::Fire);
	InputComponent->BindAxis("MoveForward", this, &ATank::MoveForward);
}

void ATank::Fire()
{
	if (AimingComp && !IsTankDestroyed())
	{
		AimingComp->FireProjectile();
	}
}

void ATank::MoveForward(float Value)
{
	if (MovementComp)
	{
		MovementComp->IntendMoveForward(Value);
	}
}


////////////////////////////////////////////////////////////////////////////////////
// Health, Damage & Death

float ATank::TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser)
{
	DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	int32 DamagePoints = FPlatformMath::RoundToInt(DamageAmount);
	int32 DamageToApply = FMath::Clamp(DamagePoints, 0, CurrentHealth + CurrentArmour);

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
		UGameplayStatics::SpawnEmitterAttached(DestroyedFX, TankBody);
		AudioComp = UGameplayStatics::SpawnSoundAttached(DestroyedSound, TankBody);
	}

	TankBody->SetCollisionProfileName("DestroyedTank");

	ATankPlayerController * const PC = this->Controller ? Cast<ATankPlayerController>(this->Controller) : nullptr;
	if (PC && EventInstigator && EventInstigator->GetPawn())
	{
		PC->LookAtLocation(EventInstigator->GetPawn()->GetActorLocation());
	}

	ABattleTankGameModeBase * const GM = GetWorld()->GetAuthGameMode<ABattleTankGameModeBase>();
	if (GM && Controller)
	{
		GM->HandleKill(Controller, EventInstigator);
	}

	OnDeath.Broadcast();
	SetLifeSpan(DestroyTimer);
}

void ATank::Execute()
{
	TakeDamage(CurrentHealth + CurrentArmour, FDamageEvent(UDamageType::StaticClass()), GetController(), nullptr);
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


////////////////////////////////////////////////////////////////////////////////////
// Update Hud

void ATank::UpdatePlayerHud()
{
	ATankPlayerController * PC = Cast<ATankPlayerController>(GetController());
	ABattleHUD * BHUD = PC ? PC->GetPlayerHud() : nullptr;
	if (BHUD)
	{
		BHUD->UpdateHealthDisplay();
	}
}

void ATank::OutOfCombatArea(bool bWarnPlayer)
{
	ATankPlayerController * PC = Cast<ATankPlayerController>(GetController());
	ABattleHUD * BHUD = PC ? PC->GetPlayerHud() : nullptr;
	if (BHUD)
	{
		bWarnPlayer ? BHUD->WarnOutOfCombatArea() : BHUD->RemoveCombatAreaWarnings();
	}
}


////////////////////////////////////////////////////////////////////////////////////
// Play FX

void ATank::OnHit(UPrimitiveComponent * HitComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{
	PlayTankCollisionFX(Hit);
}

void ATank::PlayTankCollisionFX(const FHitResult & Impact)
{
	if (Impact.bBlockingHit)
	{
		EPhysicalSurface HitSurfaceType = UPhysicalMaterial::DetermineSurfaceType(Impact.PhysMaterial.Get());
		USoundBase * ImpactSFX = GetImpactSound(HitSurfaceType);

		if (ImpactSFX)
		{
			AudioComp = UGameplayStatics::SpawnSoundAtLocation(this, ImpactSFX, Impact.ImpactPoint);
		}
	}
}

USoundBase * ATank::GetImpactSound(TEnumAsByte<EPhysicalSurface> SurfaceType) const
{
	USoundBase * CollisionSound = nullptr;

	switch (SurfaceType)
	{
	case SURFACE_METAL: CollisionSound = MetalImpactSound; break;
	case SURFACE_DIRT:  CollisionSound = LandImpactSound;  break;
	case SURFACE_GRASS: CollisionSound = LandImpactSound;  break;
	default:			CollisionSound = LandImpactSound; break;
	}
	return CollisionSound;
}

