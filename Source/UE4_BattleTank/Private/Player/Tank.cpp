// Copyright 2018 - 2022 Stuart McDonald.

#include "Tank.h"
#include "UE4_BattleTank.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "Online/BattleTankGameModeBase.h"
#include "TankPlayerController.h"
#include "AIBot/TankAISimpleMovementComp.h"
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
	MovementComp = CreateDefaultSubobject<UTankAISimpleMovementComp>(FName("MoveComponent"));

	// Setup Audio Components
	EngineAudioComp = CreateDefaultSubobject<UAudioComponent>(FName("EngineAudio"));
	EngineAudioComp->SetupAttachment(TankBody);
	EngineAudioComp->SetAutoActivate(true);

	TrackAudioComp = CreateDefaultSubobject<UAudioComponent>(FName("TrackAudio"));
	TrackAudioComp->SetupAttachment(TankBody);
	TrackAudioComp->SetAutoActivate(false);

	StressAudioComp = CreateDefaultSubobject<UAudioComponent>(FName("StressAudio"));
	StressAudioComp->SetupAttachment(TankBody);
	StressAudioComp->SetAutoActivate(false);

	StartingHealth = 100;
	StartingArmour = 50;
	DestroyTimer = 5.f;
	bHasBeenDestroyed = false;

	MaxTrackWheelSpeed = 30.f;
	TrackSpeed_Range = 1.f;

	LeftTrackUVOffset = 0.f;
	RightTrackUVOffset = 0.f;

	bIsBraking = true;
}

void ATank::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = StartingHealth;
	CurrentArmour = StartingArmour;
	UpdatePlayerHud();

	if (TankBody == NULL) { return; }
	TankBody->SetSimulatePhysics(true);
	TankBody->OnComponentHit.AddDynamic(this, &ATank::OnHit);

	SetLeftTrackMat(SetTrackMats(LeftTrackElement, TankBody->GetMaterial(LeftTrackElement)));
	SetRightTrackMat(SetTrackMats(RightTrackElement, TankBody->GetMaterial(RightTrackElement)));
}


////////////////////////////////////////////////////////////////////////////////////
// Player Input

void ATank::SetupPlayerInputComponent(UInputComponent * PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAction("Fire", IE_Pressed, this, &ATank::Fire);
	InputComponent->BindAxis("MoveForward", this, &ATank::MoveForward);
	InputComponent->BindAxis("TurnRight", this, &ATank::TurnRight);
}

void ATank::Fire()
{
	if (AimingComp != NULL && IsTankDestroyed() == false)
	{
		AimingComp->FireProjectile();
	}
}

void ATank::MoveForward(float Value)
{
	if (MovementComp != NULL)
	{
		MovementComp->IntendMoveForward(Value);
	}
}

void ATank::TurnRight(float Value)
{
	if (MovementComp != NULL)
	{
		MovementComp->IntendTurnRight(Value);
	}
}

void ATank::ApplyInputAnimationValues(float RightWheelRate, float LeftWheelRate)
{
	float CurrentLeftTrackSpeed = 0.f;
	float CurrentRightTrackSpeed = 0.f;

	if (RightWheelRate != 0.f)
	{
		CurrentRightTrackSpeed = FMath::GetMappedRangeValueClamped(FVector2D(-MaxTrackWheelSpeed, MaxTrackWheelSpeed), FVector2D(-TrackSpeed_Range, TrackSpeed_Range), RightWheelRate);
	}

	if (LeftWheelRate != 0.f)
	{
		CurrentLeftTrackSpeed = FMath::GetMappedRangeValueClamped(FVector2D(-MaxTrackWheelSpeed, MaxTrackWheelSpeed), FVector2D(-TrackSpeed_Range, TrackSpeed_Range), LeftWheelRate);
	}

	AnimateTracks(CurrentLeftTrackSpeed, CurrentRightTrackSpeed);
	TankDriveSFX(FMath::Max(RightWheelRate, LeftWheelRate));
}

void ATank::ApplyBrakes(bool bApplyBrakes)
{
	if (MovementComp != NULL)
	{
		MovementComp->SetBrakesValue(bApplyBrakes);

		if (bApplyBrakes)
		{
			if (TrackAudioComp->IsPlaying())
			{
				if (!bIsBraking)
				{
					if (StressAudioComp)
					{
						StressAudioComp->Play();
						StressAudioComp->SetFloatParameter("Speed", 300.f);
						StressAudioComp->FadeOut(1.f, 0.f);
					}
					TrackAudioComp->FadeOut(1.f, 0.f);

				}
			}

			if (EngineAudioComp)
			{
				EngineAudioComp->SetFloatParameter("RPM", 0);
			}

			bIsBraking = true;
		}
		else if (!bApplyBrakes)
		{
			bIsBraking = false;
			if (StressAudioComp)
			{
				StressAudioComp->Play();
				StressAudioComp->SetFloatParameter("Speed", 150.f);
				StressAudioComp->FadeOut(1.f, 0.f);
			}
			if (TrackAudioComp)
			{
				TrackAudioComp->Play();
			}
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////
// Health, Damage & Death

float ATank::TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser)
{
	DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	int32 DamagePoints = FPlatformMath::RoundToInt(DamageAmount);
	int32 DamageToApply = FMath::Clamp(DamagePoints, 0, CurrentHealth + CurrentArmour);

	if (IsTankDestroyed() == false)
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
	if (DestroyedFX != NULL)
	{
		UGameplayStatics::SpawnEmitterAttached(DestroyedFX, TankBody);
		CollisionAudioComp = UGameplayStatics::SpawnSoundAttached(DestroyedSound, TankBody);
	}

	StopAudioSound();

	TankBody->SetCollisionProfileName("DestroyedTank");

	ATankPlayerController * const PC = this->Controller ? Cast<ATankPlayerController>(this->Controller) : nullptr;
	if (PC != NULL && EventInstigator != NULL && EventInstigator->GetPawn() != NULL)
	{
		PC->LookAtLocation(EventInstigator->GetPawn()->GetActorLocation());
	}

	ABattleTankGameModeBase * const GM = GetWorld()->GetAuthGameMode<ABattleTankGameModeBase>();
	if (GM != NULL && Controller != NULL)
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


////////////////////////////////////////////////////////////////////////////////
// Track animation

UMaterialInstanceDynamic* ATank::SetTrackMats(int32 Element, UMaterialInterface* Mat)
{
	return Mat != NULL ? TankBody->CreateDynamicMaterialInstance(Element, Mat) : NULL;
}

void ATank::SetLeftTrackMat(UMaterialInstanceDynamic* Mat)
{
	if (Mat == NULL) { return; }
	LeftTrackMat = Mat;
}

void ATank::SetRightTrackMat(UMaterialInstanceDynamic* Mat)
{
	if (Mat == NULL) { return; }
	RightTrackMat = Mat;
}

void ATank::AnimateTracks(float LeftRangeValue, float RightRangeValue)
{
	float LeftOffset = 0;
	float RightOffset = 0;

	LeftOffset = LeftRangeValue * GetWorld()->GetDeltaSeconds();
	RightOffset = RightRangeValue * GetWorld()->GetDeltaSeconds();

	AnimateTrackMatLeft(-LeftOffset);
	AnimateTrackMatRight(-RightOffset);
}

void ATank::AnimateTrackMatLeft(float PositionOffset)
{
	if (LeftTrackMat != NULL)
	{
		LeftTrackMat->GetScalarParameterValue(TrackScalarParamName, LeftTrackUVOffset);
		LeftTrackUVOffset += PositionOffset;
		LeftTrackMat->SetScalarParameterValue(TrackScalarParamName, LeftTrackUVOffset);

		// Don't let Scalar value get too high, or material will distort
		if (LeftTrackUVOffset < -10000.f || LeftTrackUVOffset > 10000.f)
		{
			LeftTrackMat->SetScalarParameterValue(TrackScalarParamName, 0.f);
		}
	}
}

void ATank::AnimateTrackMatRight(float PositionOffset)
{
	if (RightTrackMat != NULL)
	{
		RightTrackMat->GetScalarParameterValue(TrackScalarParamName, RightTrackUVOffset);
		RightTrackUVOffset += PositionOffset;
		RightTrackMat->SetScalarParameterValue(TrackScalarParamName, RightTrackUVOffset);

		// Don't let Scalar value get too high, or material will distort
		if (RightTrackUVOffset < -10000.f || RightTrackUVOffset > 10000.f)
		{
			RightTrackMat->SetScalarParameterValue(TrackScalarParamName, 0.f);
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////
// Update Hud

void ATank::UpdatePlayerHud()
{
	ATankPlayerController * PC = Cast<ATankPlayerController>(GetController());
	ABattleHUD * BHUD = PC ? PC->GetPlayerHud() : nullptr;
	if (BHUD != NULL)
	{
		BHUD->UpdateHealthDisplay();
	}
}

void ATank::OutOfCombatArea(bool bWarnPlayer)
{
	ATankPlayerController * PC = Cast<ATankPlayerController>(GetController());
	ABattleHUD * BHUD = PC ? PC->GetPlayerHud() : nullptr;
	if (BHUD != NULL)
	{
		bWarnPlayer ? BHUD->WarnOutOfCombatArea() : BHUD->RemoveCombatAreaWarnings();
	}
}


////////////////////////////////////////////////////////////////////////////////////
// Play FX

void ATank::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	
	PlayTankCollisionFX(HighImpactSound);
}

void ATank::PlayTankCollisionFX(USoundBase* ImpactSFX)
{
	if (ImpactSFX != NULL)
	{
		CollisionAudioComp = UGameplayStatics::SpawnSoundAttached(ImpactSFX, TankBody);
	}
}

void ATank::TankDriveSFX(float TrackSpeed)
{
	if (MovementComp == NULL) { return; }

	float Speed = FMath::Abs(MovementComp->GetForwardTorque());

	if (EngineAudioComp)
	{
		EngineAudioComp->SetFloatParameter("RPM", Speed);
	}
	if (TrackAudioComp)
	{
		if (TrackAudioComp->IsPlaying())
		{
			TrackAudioComp->SetFloatParameter("Speed", TrackSpeed);
		}
	}

	CurrentSpeed = Speed;
}

void ATank::StopAudioSound()
{
	if (CollisionAudioComp && CollisionAudioComp->IsPlaying())
	{
		CollisionAudioComp->Stop();
	}
	if (EngineAudioComp && EngineAudioComp->IsPlaying())
	{
		EngineAudioComp->Stop();
	}
	if (TrackAudioComp && TrackAudioComp->IsPlaying())
	{
		TrackAudioComp->Stop();
	}
}

