// Copyright 2018 Stuart McDonald.

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

	if (TankBody == NULL) { return; }
	TankBody->SetSimulatePhysics(true);
	TankBody->OnComponentHit.AddDynamic(this, &ATank::OnHit);

	EngineAudio = SFXPlay(EngineLoopSfx);

	if (TrackMat == NULL) { return; }
	SetLeftTrackMat(SetTrackMats(LeftTrackElement, TrackMat));
	SetRightTrackMat(SetTrackMats(RightTrackElement, TrackMat));
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
		MoveForwardValue = FMath::Clamp<float>(Value, -1, 1);
		MovementComp->IntendMoveForward(Value);
	}

	ApplyInputMovementBehaviours();
}

void ATank::TurnRight(float Value)
{
	if (MovementComp != NULL)
	{
		TurnRightValue = FMath::Clamp<float>(Value, -1, 1);

		float MaxSpeed = FGenericPlatformMath::Abs<float>(TurnRightValue * TurnRate);
		float MaxTurnSpeed = TurnRightValue * TurnRate;

		TurnSpeed += MaxTurnSpeed * GetWorld()->DeltaTimeSeconds;
		TurnSpeed = FMath::Clamp<float>(TurnSpeed, -MaxSpeed, MaxSpeed);

		MovementComp->IntendTurnRight(Value);
	}
}

void ATank::ApplyInputMovementBehaviours()
{
	float ForwardVelocity = FVector::DotProduct(GetActorForwardVector(), GetVelocity());
	float ForwardSpeed = FMath::GetMappedRangeValueClamped(FVector2D(-700.f, 700.f), FVector2D(-1, 1), ForwardVelocity);

	FRotator NewYawRot = GetActorRotation();
	float YawRotSpeed = FMath::FindDeltaAngleDegrees(NewYawRot.Yaw, LastYawRot.Yaw);
	LastYawRot = NewYawRot;
	float TurningSpeed = FMath::GetMappedRangeValueClamped(FVector2D(-TurnRate, TurnRate), FVector2D(-1, 1), YawRotSpeed);

	AnimateTracks(ForwardSpeed, TurningSpeed);
	TurnWheels(ForwardSpeed, TurnSpeed);

	float EnginePitch = FMath::GetMappedRangeValueClamped(FVector2D(0.f, 650.f), FVector2D(MinSoundPitch, MaxSoundPitch), FGenericPlatformMath::Abs<float>(ForwardVelocity));
	TankSFXPitch(FGenericPlatformMath::Abs<float>(EnginePitch));
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
		AudioComp = UGameplayStatics::SpawnSoundAttached(DestroyedSound, TankBody);
	}

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


////////////////////////////////////////////////////////////////////////////////
// Wheel animation

void ATank::TurnWheels(float ForwardSpeed, float TurnSpeed)
{
	float WheelSpeedYaw = 0.f;
	if (MoveForwardValue != 0.f)
	{
		WheelSpeedYaw = SetWheelTurnValue(ForwardSpeed);

		LeftFrontBackYaw -= WheelSpeedYaw;
		RightFrontBackYaw -= WheelSpeedYaw;
	}
	else if (TurnRightValue != 0.f)
	{
		WheelSpeedYaw = SetWheelTurnValue(TurnSpeed);

		LeftWheelYaw -= WheelSpeedYaw;
		LeftFrontBackYaw -= WheelSpeedYaw;

		RightWheelYaw += WheelSpeedYaw;
		RightFrontBackYaw += WheelSpeedYaw;
	}
}

float ATank::SetWheelTurnValue(float TurnSpeed)
{
	float WheelSpeedYaw = TurnSpeed * WheelTurnMultiplier;

	// Don't let WheelTurnYaw get too high.
	if (LeftFrontBackYaw <= -36000.f || LeftFrontBackYaw >= 36000.f)
	{
		LeftFrontBackYaw = 0.f;
	}
	if (RightFrontBackYaw <= -36000.f || RightFrontBackYaw >= 36000.f)
	{
		RightFrontBackYaw = 0.f;
	}
	return WheelSpeedYaw;
}


////////////////////////////////////////////////////////////////////////////////
// Track animation

UMaterialInstanceDynamic * ATank::SetTrackMats(int32 Element, UMaterialInterface * Mat)
{
	return Mat != NULL ? TankBody->CreateDynamicMaterialInstance(Element, Mat) : NULL;
}

void ATank::SetLeftTrackMat(UMaterialInstanceDynamic * TrackMat)
{
	if (TrackMat == NULL) { return; }
	LeftTrackMat = TrackMat;
}

void ATank::SetRightTrackMat(UMaterialInstanceDynamic * TrackMat)
{
	if (TrackMat == NULL) { return; }
	RightTrackMat = TrackMat;
}

void ATank::AnimateTracks(float ForwardSpeed, float TurnSpeed)
{
	float TrackOffset = 0;

	if (MoveForwardValue != 0.f)
	{
		float MaxSpeed = ForwardSpeed * GetWorld()->GetDeltaSeconds();
		TrackOffset = MaxSpeed * TrackSpeedMultiplier;

		AnimateTrackMatLeft(-TrackOffset);
		AnimateTrackMatRight(-TrackOffset);
	}
	else if (TurnRightValue != 0.f)
	{
		TrackOffset = TurnSpeed * GetWorld()->GetDeltaSeconds();

		AnimateTrackMatLeft(TrackOffset);
		AnimateTrackMatRight(-TrackOffset);
	}
}

void ATank::AnimateTrackMatLeft(float NewOffset)
{
	if (LeftTrackMat != NULL)
	{
		float CurrentOffset;
		LeftTrackMat->GetScalarParameterValue(TrackScalarParamName, CurrentOffset);
		NewOffset += CurrentOffset;

		LeftTrackMat->SetScalarParameterValue(TrackScalarParamName, NewOffset);

		// Don't let Scalar value get too high.
		if (CurrentOffset < -10000.f || CurrentOffset > 10000.f)
		{
			LeftTrackMat->SetScalarParameterValue(TrackScalarParamName, 0.f);
		}
	}
}

void ATank::AnimateTrackMatRight(float NewOffset)
{
	if (RightTrackMat != NULL)
	{
		float CurrentOffset;
		RightTrackMat->GetScalarParameterValue(TrackScalarParamName, CurrentOffset);
		NewOffset += CurrentOffset;

		RightTrackMat->SetScalarParameterValue(TrackScalarParamName, NewOffset);

		// Don't let Scalar value get too high.
		if (CurrentOffset < -10000.f || CurrentOffset > 10000.f)
		{
			RightTrackMat->SetScalarParameterValue(TrackScalarParamName, 0.f);
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////
// Play FX

void ATank::OnHit(UPrimitiveComponent * HitComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{
	if (OtherActor != NULL)
	{
		PlayTankCollisionFX(Hit);
	}
}

void ATank::PlayTankCollisionFX(const FHitResult & Impact)
{
	if (Impact.bBlockingHit == true)
	{
		EPhysicalSurface HitSurfaceType = UPhysicalMaterial::DetermineSurfaceType(Impact.PhysMaterial.Get());
		USoundBase * ImpactSFX = GetImpactSound(HitSurfaceType);

		if (ImpactSFX != NULL)
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

void ATank::TankSFXPitch(float PitchRange)
{
	if (EngineAudio)
	{
		EngineAudio->SetPitchMultiplier(PitchRange);
	}
}

UAudioComponent * ATank::SFXPlay(USoundBase * SoundFX)
{
	UAudioComponent * AC = nullptr;
	if (SoundFX)
	{
		AC = UGameplayStatics::SpawnSoundAttached(SoundFX, GetRootComponent());
		AC->FadeIn(0.1f, 1.f);
	}
	return AC;
}

void ATank::StopEngineSound()
{
	if (EngineAudio && EngineAudio->IsPlaying())
	{
		EngineAudio->Stop();
	}
}

