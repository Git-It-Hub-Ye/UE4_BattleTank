// Copyright 2018 - 2021 Stuart McDonald.

#include "TankVehicle.h"
#include "UE4_BattleTank.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "TimerManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "Online/BattleTankGameModeBase.h"
#include "TankPlayerController.h"
#include "TankVehicleMovementComponent.h"
#include "AimingComponent.h"
#include "UI/BattleHUD.h"


ATankVehicle::ATankVehicle(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UTankVehicleMovementComponent>(VehicleMovementComponentName))
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Will set physics after spawn in begin play to avoid teleport flag errors.
	GetMesh()->SetSimulatePhysics(false);

	AimingComp = CreateDefaultSubobject<UAimingComponent>(FName("AimComponent"));

	StartingHealth = 100;
	StartingArmour = 50;
	DestroyTimer = 5.f;
	bHasBeenDestroyed = false;

	MaxTrackForwardSpeed = 750.f;
	TrackForwardSpeed_Range = 2.f;
	MaxTrackTurnSpeed = 0.4f;
	TrackTurnSpeed_Range = 1.f;

	LeftTrackUVOffset = 0.f;
	RightTrackUVOffset = 0.f;
}

void ATankVehicle::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = StartingHealth;
	CurrentArmour = StartingArmour;
	UpdatePlayerHud();

	if (GetMesh() == NULL) { return; }
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->OnComponentHit.AddDynamic(this, &ATankVehicle::OnHit);

	SetMovementComp();

	EngineAudio = SFXPlay(EngineLoopSfx);

	SetLeftTrackMat(SetTrackMats(LeftTrackElement, GetMesh()->GetMaterial(LeftTrackElement)));
	SetRightTrackMat(SetTrackMats(RightTrackElement, GetMesh()->GetMaterial(RightTrackElement)));
}


////////////////////////////////////////////////////////////////////////////////////
// Player Input

void ATankVehicle::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAction("Fire", IE_Pressed, this, &ATankVehicle::Fire);
	InputComponent->BindAxis("MoveForward", this, &ATankVehicle::MoveForward);
	InputComponent->BindAxis("TurnRight", this, &ATankVehicle::TurnRight);
}

void ATankVehicle::Fire()
{
	if (AimingComp != NULL && IsTankDestroyed() == false)
	{
		AimingComp->FireProjectile();
	}
}

void ATankVehicle::MoveForward(float Value)
{
	if (MovementComp != NULL)
	{
		MovementComp->SetThrottleInput(Value);
	}
	else
	{
		SetMovementComp();
	}
	ApplyInputMovementBehaviours();
}

void ATankVehicle::TurnRight(float Value)
{
	if (MovementComp != NULL)
	{
		MovementComp->SetSteeringInput(Value);
	}
	else
	{
		SetMovementComp();
	}
}

void ATankVehicle::ApplyBrakes(bool bApplyBrake)
{
	if (MovementComp != NULL)
	{
	}
	else
	{
		SetMovementComp();
	}
}

void ATankVehicle::ApplyInputMovementBehaviours()
{
	if (MovementComp == NULL) { return; }
	ForwardSpeedValue = FMath::GetMappedRangeValueClamped(FVector2D(-MaxTrackForwardSpeed, MaxTrackForwardSpeed), FVector2D(-TrackForwardSpeed_Range, TrackForwardSpeed_Range), MovementComp->GetForwardSpeed());

	FRotator NewYawRot = GetActorRotation();
	float YawRotSpeed = FMath::FindDeltaAngleDegrees(NewYawRot.Yaw, LastYawRot.Yaw);
	LastYawRot = NewYawRot;
	TurnSpeedValue = FMath::GetMappedRangeValueClamped(FVector2D(-MaxTrackTurnSpeed, MaxTrackTurnSpeed), FVector2D(-TrackTurnSpeed_Range, TrackTurnSpeed_Range), YawRotSpeed);

	AnimateTracks(ForwardSpeedValue, TurnSpeedValue);
	
	float EnginePitch = FMath::GetMappedRangeValueClamped(FVector2D(0.f, 650.f), FVector2D(MinSoundPitch, MaxSoundPitch), FGenericPlatformMath::Abs<float>(MovementComp->GetForwardSpeed()));
	TankSFXPitch(FGenericPlatformMath::Abs<float>(EnginePitch));
}

void ATankVehicle::SetMovementComp()
{
	MovementComp = Cast<UTankVehicleMovementComponent>(GetMovementComponent());
}


////////////////////////////////////////////////////////////////////////////////////
// Health, Damage & Death

float ATankVehicle::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
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

void ATankVehicle::OnDeathBehaviour(AController* EventInstigator)
{
	ApplyBrakes(true);

	if (DestroyedFX != NULL)
	{
		UGameplayStatics::SpawnEmitterAttached(DestroyedFX, GetMesh());
		AudioComp = UGameplayStatics::SpawnSoundAttached(DestroyedSound, GetMesh());
	}

	GetMesh()->SetCollisionProfileName("DestroyedTank");

	ATankPlayerController* const PC = this->Controller ? Cast<ATankPlayerController>(this->Controller) : nullptr;
	if (PC != NULL && EventInstigator != NULL && EventInstigator->GetPawn() != NULL)
	{
		PC->LookAtLocation(EventInstigator->GetPawn()->GetActorLocation());
	}

	ABattleTankGameModeBase* const GM = GetWorld()->GetAuthGameMode<ABattleTankGameModeBase>();
	if (GM != NULL && Controller != NULL)
	{
		GM->HandleKill(Controller, EventInstigator);
	}

	//OnDeath.Broadcast();
	SetLifeSpan(DestroyTimer);
}

void ATankVehicle::Execute()
{
	TakeDamage(CurrentHealth + CurrentArmour, FDamageEvent(UDamageType::StaticClass()), GetController(), nullptr);
}

void ATankVehicle::ReplenishHealth(float HealthToAdd)
{
	CurrentHealth = CurrentHealth + HealthToAdd;
	CurrentHealth = FMath::Clamp<int32>(CurrentHealth, 0, StartingHealth);
	UpdatePlayerHud();
}

void ATankVehicle::ReplenishArmour()
{
	CurrentArmour = StartingArmour;
	UpdatePlayerHud();
}

////////////////////////////////////////////////////////////////////////////////
// Track animation

UMaterialInstanceDynamic* ATankVehicle::SetTrackMats(int32 Element, UMaterialInterface* Mat)
{
	return Mat != NULL ? GetMesh()->CreateDynamicMaterialInstance(Element, Mat) : NULL;
}

void ATankVehicle::SetLeftTrackMat(UMaterialInstanceDynamic* Mat)
{
	if (Mat == NULL) { return; }
	LeftTrackMat = Mat;
	LeftTrackMat->SetScalarParameterValue(TrackScalarParamName, -9990);
}

void ATankVehicle::SetRightTrackMat(UMaterialInstanceDynamic* Mat)
{
	if (Mat == NULL) { return; }
	RightTrackMat = Mat;
	RightTrackMat->SetScalarParameterValue(TrackScalarParamName,-9990);
}

void ATankVehicle::AnimateTracks(float ForwardRangeValue, float RotationRangeValue)
{
	if (MovementComp == NULL) { return; }
	float ForwardOffset = 0;
	float TurnOffset = 0;

	ForwardOffset = ForwardRangeValue * GetWorld()->GetDeltaSeconds();
	TurnOffset = RotationRangeValue * GetWorld()->GetDeltaSeconds();

	AnimateTrackMatLeft(-ForwardOffset + TurnOffset);
	AnimateTrackMatRight(-ForwardOffset - TurnOffset);
}

void ATankVehicle::AnimateTrackMatLeft(float PositionOffset)
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

void ATankVehicle::AnimateTrackMatRight(float PositionOffset)
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

void ATankVehicle::UpdatePlayerHud()
{
	ATankPlayerController* PC = Cast<ATankPlayerController>(GetController());
	ABattleHUD* BHUD = PC ? PC->GetPlayerHud() : nullptr;
	if (BHUD != NULL)
	{
		BHUD->UpdateHealthDisplay();
	}
}

void ATankVehicle::OutOfCombatArea(bool bWarnPlayer)
{
	ATankPlayerController* PC = Cast<ATankPlayerController>(GetController());
	ABattleHUD* BHUD = PC ? PC->GetPlayerHud() : nullptr;
	if (BHUD != NULL)
	{
		bWarnPlayer ? BHUD->WarnOutOfCombatArea() : BHUD->RemoveCombatAreaWarnings();
	}
}


////////////////////////////////////////////////////////////////////////////////////
// Play FX

void ATankVehicle::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != NULL)
	{
		PlayTankCollisionFX(Hit);
	}
}

void ATankVehicle::PlayTankCollisionFX(const FHitResult& Impact)
{
	if (Impact.bBlockingHit == true)
	{
		EPhysicalSurface HitSurfaceType = UPhysicalMaterial::DetermineSurfaceType(Impact.PhysMaterial.Get());
		USoundBase* ImpactSFX = GetImpactSound(HitSurfaceType);

		if (ImpactSFX != NULL)
		{
			AudioComp = UGameplayStatics::SpawnSoundAtLocation(this, ImpactSFX, Impact.ImpactPoint);
		}
	}
}

USoundBase* ATankVehicle::GetImpactSound(TEnumAsByte<EPhysicalSurface> SurfaceType) const
{
	USoundBase* CollisionSound = nullptr;

	switch (SurfaceType)
	{
	case SURFACE_METAL: CollisionSound = MetalImpactSound; break;
	case SURFACE_DIRT:  CollisionSound = LandImpactSound;  break;
	case SURFACE_GRASS: CollisionSound = LandImpactSound;  break;
	default:			CollisionSound = LandImpactSound; break;
	}
	return CollisionSound;
}

void ATankVehicle::TankSFXPitch(float PitchRange)
{
	if (EngineAudio)
	{
		EngineAudio->SetPitchMultiplier(PitchRange);
	}
}

UAudioComponent* ATankVehicle::SFXPlay(USoundBase* SoundFX)
{
	UAudioComponent* AC = nullptr;
	if (SoundFX)
	{
		AC = UGameplayStatics::SpawnSoundAttached(SoundFX, GetRootComponent());
		AC->FadeIn(0.1f, 1.f);
	}
	return AC;
}

void ATankVehicle::StopEngineSound()
{
	if (EngineAudio && EngineAudio->IsPlaying())
	{
		EngineAudio->Stop();
	}
}

