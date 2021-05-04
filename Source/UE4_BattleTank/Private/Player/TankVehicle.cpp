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

	if (TrackMat == NULL) { return; }
	SetLeftTrackMat(SetTrackMats(LeftTrackElement, TrackMat));
	SetRightTrackMat(SetTrackMats(RightTrackElement, TrackMat));
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
	}
	else
	{
		SetMovementComp();
	}
}

void ATankVehicle::TurnRight(float Value)
{
	if (MovementComp != NULL)
	{
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

void ATankVehicle::ApplyInputMovementBehaviours(float TurnRate, float TurnSpeed)
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
// Wheel animation

void ATankVehicle::TurnWheels(float ForwardSpeed, float TurnSpeed)
{
	if (MovementComp == NULL) { return; }

	float WheelSpeedYaw = 0.f;
	/*if (MovementComp->GetForwardValue() != 0.f)
	{
		WheelSpeedYaw = SetWheelTurnValue(ForwardSpeed);

		LeftFrontBackYaw -= WheelSpeedYaw + TurnSpeed;
		RightFrontBackYaw -= WheelSpeedYaw - TurnSpeed;
	}
	else if (MovementComp->GetTurnRightValue() != 0.f)
	{
		WheelSpeedYaw = SetWheelTurnValue(TurnSpeed);

		LeftWheelYaw -= WheelSpeedYaw;
		LeftFrontBackYaw -= WheelSpeedYaw;

		RightWheelYaw += WheelSpeedYaw;
		RightFrontBackYaw += WheelSpeedYaw;
	}*/
}

float ATankVehicle::SetWheelTurnValue(float TurnSpeed)
{
	float WheelSpeedYaw = TurnSpeed * TurnSpeedMultiplier;

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

UMaterialInstanceDynamic* ATankVehicle::SetTrackMats(int32 Element, UMaterialInterface* Mat)
{
	return Mat != NULL ? GetMesh()->CreateDynamicMaterialInstance(Element, Mat) : NULL;
}

void ATankVehicle::SetLeftTrackMat(UMaterialInstanceDynamic* Mat)
{
	if (Mat == NULL) { return; }
	LeftTrackMat = Mat;
}

void ATankVehicle::SetRightTrackMat(UMaterialInstanceDynamic* Mat)
{
	if (Mat == NULL) { return; }
	RightTrackMat = Mat;
}

void ATankVehicle::AnimateTracks(float ForwardSpeed, float TurnSpeed)
{
	if (MovementComp == NULL) { return; }
	float TrackOffset = 0;

	/*if (MovementComp->GetForwardValue() != 0.f)
	{
		TrackOffset = ForwardSpeed * TurnSpeedMultiplier * GetWorld()->GetDeltaSeconds();
		float TurnModifier = TurnSpeed * GetWorld()->GetDeltaSeconds();

		AnimateTrackMatLeft(-TrackOffset + TurnModifier);
		AnimateTrackMatRight(-TrackOffset - TurnModifier);
	}
	else if (MovementComp->GetTurnRightValue() != 0.f)
	{
		TrackOffset = TurnSpeed * GetWorld()->GetDeltaSeconds();

		AnimateTrackMatLeft(TrackOffset);
		AnimateTrackMatRight(-TrackOffset);
	}*/
}

void ATankVehicle::AnimateTrackMatLeft(float NewOffset)
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

void ATankVehicle::AnimateTrackMatRight(float NewOffset)
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

