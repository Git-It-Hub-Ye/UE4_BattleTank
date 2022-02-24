// Copyright 2018 - 2022 Stuart McDonald.

#include "TankVehicle.h"
#include "UE4_BattleTank.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "TimerManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"

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

	// Create Tank Components
	AimingComp = CreateDefaultSubobject<UAimingComponent>(FName("AimComponent"));

	SFXVolume = CreateDefaultSubobject<USphereComponent >(FName("SFX Projectile Volume"));
	SFXVolume->SetupAttachment(GetMesh());
	SFXVolume->SetGenerateOverlapEvents(true);
	SFXVolume->bHiddenInGame = true;
	SFXVolume->SetWorldScale3D(FVector(50.f, 50.f, 50.f));

	// Setup Audio Components
	EngineAudioComp = CreateDefaultSubobject<UAudioComponent>(FName("EngineAudio"));
	EngineAudioComp->SetupAttachment(GetMesh());
	EngineAudioComp->SetAutoActivate(true);

	TrackAudioComp = CreateDefaultSubobject<UAudioComponent>(FName("TrackAudio"));
	TrackAudioComp->SetupAttachment(GetMesh());
	TrackAudioComp->SetAutoActivate(false);

	StressAudioComp = CreateDefaultSubobject<UAudioComponent>(FName("StressAudio"));
	StressAudioComp->SetupAttachment(GetMesh());
	StressAudioComp->SetAutoActivate(false);

	// Setup Particle Components
	ExhaustFX = CreateDefaultSubobject<UParticleSystemComponent>(FName("ExhaustFX"));
	ExhaustFX->SetupAttachment(GetMesh());
	ExhaustFX->SetAutoActivate(false);

	LowHealthFX = CreateDefaultSubobject<UParticleSystemComponent>(FName("LowHealthFX"));
	LowHealthFX->SetupAttachment(GetMesh());
	LowHealthFX->SetAutoActivate(false);

	DestroyedFX = CreateDefaultSubobject<UParticleSystemComponent>(FName("DestroyedFX"));
	DestroyedFX->SetupAttachment(GetMesh());
	DestroyedFX->SetAutoActivate(false);

	// Left Track Setup Particle Components
	LeftTrackKickupFX_Front = CreateDefaultSubobject<UParticleSystemComponent>(FName("Left Track Kickup FX Front"));
	LeftTrackKickupFX_Front->SetupAttachment(GetMesh(), FName("L_Track_1"));
	LeftTrackKickupFX_Front->SetAutoActivate(false);

	LeftTrackKickupFX_Rear = CreateDefaultSubobject<UParticleSystemComponent>(FName("Left Track Kickup FX Rear"));
	LeftTrackKickupFX_Rear->SetupAttachment(GetMesh(), FName("L_Track_5"));
	LeftTrackKickupFX_Rear->SetAutoActivate(false);

	LeftTrackDebrisFX_Front = CreateDefaultSubobject<UParticleSystemComponent>(FName("Left Track Debris FX Front"));
	LeftTrackDebrisFX_Front->SetupAttachment(GetMesh());
	LeftTrackDebrisFX_Front->SetAutoActivate(false);

	LeftTrackDebrisFX_Rear = CreateDefaultSubobject<UParticleSystemComponent>(FName("Left Track Debris FX Rear"));
	LeftTrackDebrisFX_Rear->SetupAttachment(GetMesh());
	LeftTrackDebrisFX_Rear->SetAutoActivate(false);

	// Right Track Setup Particle Components
	RightTrackKickupFX_Front = CreateDefaultSubobject<UParticleSystemComponent>(FName("Right Track Kickup FX Front"));
	RightTrackKickupFX_Front->SetupAttachment(GetMesh(), FName("R_Track_1"));
	RightTrackKickupFX_Front->SetAutoActivate(false);

	RightTrackKickupFX_Rear = CreateDefaultSubobject<UParticleSystemComponent>(FName("Right Track Kickup FX Rear"));
	RightTrackKickupFX_Rear->SetupAttachment(GetMesh(), FName("R_Track_5"));
	RightTrackKickupFX_Rear->SetAutoActivate(false);

	RightTrackDebrisFX_Front = CreateDefaultSubobject<UParticleSystemComponent>(FName("Right Track Debris FX Front"));
	RightTrackDebrisFX_Front->SetupAttachment(GetMesh());
	RightTrackDebrisFX_Front->SetAutoActivate(false);

	RightTrackDebrisFX_Rear = CreateDefaultSubobject<UParticleSystemComponent>(FName("Right Track Debris FX Rear"));
	RightTrackDebrisFX_Rear->SetupAttachment(GetMesh());
	RightTrackDebrisFX_Rear->SetAutoActivate(false);

	// Setup default variables
	StartingHealth = 100;
	StartingArmour = 50;
	LowHealthThreshold = 25;
	DestroyTimer = 5.f;
	bHasBeenDestroyed = false;

	MaxTrackWheelSpeed = 30.f;
	TrackSpeed_Range = 1.f;

	LeftTrackUVOffset = 0.f;
	RightTrackUVOffset = 0.f;

	bIsBraking = true;
}

void ATankVehicle::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = StartingHealth;
	CurrentArmour = StartingArmour;
	UpdatePlayerHud();
	ExhaustFX->Activate();

	if (GetMesh() == NULL) { return; }
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->OnComponentHit.AddDynamic(this, &ATankVehicle::OnHit);

	SetMovementComp();

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

	ApplyInputAnimationValues();
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

void ATankVehicle::ApplyInputAnimationValues()
{
	// Only do calculations when brakes are not applied
	if (MovementComp != NULL && MovementComp->GetBrakeInputValue() != 1.f)
	{
		LeftWheelSpeedValue = FMath::GetMappedRangeValueClamped(FVector2D(-MaxTrackWheelSpeed, MaxTrackWheelSpeed), FVector2D(-TrackSpeed_Range, TrackSpeed_Range), MovementComp->GetLeftWheelSpeed());
		RightWheelSpeedValue = FMath::GetMappedRangeValueClamped(FVector2D(-MaxTrackWheelSpeed, MaxTrackWheelSpeed), FVector2D(-TrackSpeed_Range, TrackSpeed_Range), MovementComp->GetRightWheelSpeed());
	
		AnimateTracks(LeftWheelSpeedValue, RightWheelSpeedValue);
		SFXTrackSpeedValue = FMath::Max(FMath::Abs(MovementComp->GetLeftWheelSpeed()), FMath::Abs(MovementComp->GetRightWheelSpeed()));

		TankParticleFX(MovementComp->GetThrottleInputValue(), MovementComp->GetSteeringInputValue(), MovementComp->GetLeftWheelSpeed(), MovementComp->GetRightWheelSpeed());
	}
	else
	{
		AnimateTracks(0.f, 0.f);
	}

	TankDriveSFX();
}

void ATankVehicle::SetMovementComp()
{
	MovementComp = Cast<UTankVehicleMovementComponent>(GetMovementComponent());
}

void ATankVehicle::SwitchCameraView(bool bThirdPersonView, bool bZoomMode)
{
	ATankPlayerController* const PC = this->Controller ? Cast<ATankPlayerController>(this->Controller) : nullptr;
	ABattleHUD* BHUD = PC ? PC->GetPlayerHud() : nullptr;

	if (BHUD != NULL)
	{
		BHUD->UpdateCrosshairDisplay(bThirdPersonView, bZoomMode);
	}
}


////////////////////////////////////////////////////////////////////////////////////
// Tank Behaviour

float ATankVehicle::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	int32 DamagePoints = FPlatformMath::RoundToInt(DamageAmount);
	int32 DamageToApply = FMath::Clamp(DamagePoints, 0, CurrentHealth + CurrentArmour);

	if (DamageToApply > 0)
	{
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

			UpdateDamageIndicator(EventInstigator->GetPawn());

			if (CurrentHealth <= 0)
			{
				bHasBeenDestroyed = true;
				OnDeathBehaviour(EventInstigator);
			}
			else if (CurrentHealth <= LowHealthThreshold)
			{
				LowHealthFX->Activate();
			}
		}
	}
	return DamageToApply;
}

void ATankVehicle::OnDeathBehaviour(AController* EventInstigator)
{
	//ApplyBrakes(true);
	
	DestroyedFX->Activate();
	CollisionAudioComp = UGameplayStatics::SpawnSoundAttached(DestroyedSound, GetMesh());

	StopAudioSound();
	ExhaustFX->DeactivateSystem();

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

	if (CurrentHealth > LowHealthThreshold)
	{
		LowHealthFX->DeactivateSystem();
	}

	UpdatePlayerHud();
}

void ATankVehicle::ReplenishArmour()
{
	CurrentArmour = StartingArmour;
	UpdatePlayerHud();
}

void ATankVehicle::ActivateFireImpulse(FVector Torque)
{
	GetMesh()->AddImpulse(Torque, "T-62_body");
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

void ATankVehicle::AnimateTracks(float LeftRangeValue, float RightRangeValue)
{
	if (MovementComp == NULL) { return; }
	float LeftOffset = 0;
	float RightOffset = 0;

	LeftOffset = LeftRangeValue * GetWorld()->GetDeltaSeconds();
	RightOffset = RightRangeValue * GetWorld()->GetDeltaSeconds();

	AnimateTrackMatLeft(-LeftOffset);
	AnimateTrackMatRight(-RightOffset);
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
	ATankPlayerController * PC = Cast<ATankPlayerController>(GetController());
	ABattleHUD* BHUD = PC ? PC->GetPlayerHud() : nullptr;
	if (BHUD != NULL)
	{
		BHUD->UpdateHealthDisplay();
	}
}

void ATankVehicle::UpdateDamageIndicator(AActor* DamageCauser)
{
	ATankPlayerController * PC = Cast<ATankPlayerController>(GetController());
	ABattleHUD* BHUD = PC ? PC->GetPlayerHud() : nullptr;
	if (BHUD != NULL)
	{
		BHUD->UpdateHealthDisplay();
		BHUD->UpdateDamageIndicator(DamageCauser);
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
	if (FMath::Abs(MovementComp->GetForwardSpeed()) > 250.f)
	{
		PlayTankCollisionFX(HighImpactSound);
	}
	else if (FMath::Abs(MovementComp->GetForwardSpeed()) > 50.f || SFXTrackSpeedValue > 5.f)
	{
		PlayTankCollisionFX(LowImpactSound);
	}
}

void ATankVehicle::PlayTankCollisionFX(USoundBase* ImpactSFX)
{
	if (ImpactSFX != NULL)
	{
		CollisionAudioComp = UGameplayStatics::SpawnSoundAttached(ImpactSFX, GetMesh());
	}
}

void ATankVehicle::TankDriveSFX()
{
	if (MovementComp == NULL) { return; }
	if (EngineAudioComp)
	{
		EngineAudioComp->SetFloatParameter("RPM", MovementComp->GetEngineRotationSpeed());
	}

	if (TrackAudioComp)
	{
		if (MovementComp->GetThrottleInputValue() != 0.f && bIsBraking == true)
		{
			bIsBraking = false;
			if (StressAudioComp && !StressAudioComp->IsPlaying())
			{
				StressAudioComp->Play();
				StressAudioComp->SetFloatParameter("Speed", 75.f);
				StressAudioComp->FadeOut(FadeOutTime_Stress, 0.f);
			}
			TrackAudioComp->Play();
		}
		else if (TrackAudioComp->IsPlaying())
		{
			if (!bIsBraking && MovementComp->GetThrottleInputValue() == 0.f)
			{
				if (FMath::Abs(MovementComp->GetForwardSpeed()) >= 300.f && StressAudioComp)
				{
					StressAudioComp->Play();
					StressAudioComp->SetFloatParameter("Speed", MovementComp->GetForwardSpeed());
					StressAudioComp->FadeOut(FadeOutTime_Stress, 0.f);
				}
				bIsBraking = true;
				TrackAudioComp->FadeOut(FadeOutTime_Track, 0.f);
			}
			else
			{
				TrackAudioComp->SetFloatParameter("Speed", SFXTrackSpeedValue);
			}
		}
	}
}

void ATankVehicle::StopAudioSound()
{
	if (EngineAudioComp && EngineAudioComp->IsPlaying())
	{
		EngineAudioComp->Stop();
	}
	if (TrackAudioComp && TrackAudioComp->IsPlaying())
	{
		TrackAudioComp->Stop();
	}
}

void ATankVehicle::TankParticleFX(float Throttle, float Steering, float LeftWheelSpeedVal, float RightWheelSpeedVal)
{
	float Steer = LeftWheelSpeedVal;

	ExhaustFX->SetVectorParameter("Velocity", FVector(Steer, Throttle, 0.f));
	ExhaustFX->SetFloatParameter("SpawnRate", Throttle);

	LeftTrackParticleFX(LeftWheelSpeedVal);
	RightTrackParticleFX(RightWheelSpeedVal);
}

void ATankVehicle::LeftTrackParticleFX(float LeftWheelSpeedVal)
{
	if (LeftWheelSpeedVal > 0.f)
	{
		if (LeftTrackKickupFX_Rear->IsActive() && LeftTrackDebrisFX_Front->IsActive())
		{
			LeftTrackKickupFX_Rear->SetVectorParameter("WheelSpeed", FVector(LeftWheelSpeedVal));
			LeftTrackKickupFX_Rear->SetFloatParameter("WheelSpeed", LeftWheelSpeedVal);

			LeftTrackDebrisFX_Front->SetVectorParameter("WheelSpeed", FVector(LeftWheelSpeedVal));
			LeftTrackDebrisFX_Front->SetFloatParameter("WheelSpeed", LeftWheelSpeedVal);
		}
		else
		{
			LeftTrackKickupFX_Rear->Activate();
			LeftTrackDebrisFX_Front->Activate();
		}

		if (LeftTrackKickupFX_Front->IsActive() || LeftTrackDebrisFX_Rear->IsActive())
		{
			LeftTrackKickupFX_Front->Deactivate();
			LeftTrackDebrisFX_Rear->Deactivate();
		}
	}
	else if (LeftWheelSpeedVal < 0.f)
	{
		if (LeftTrackKickupFX_Front->IsActive() && LeftTrackDebrisFX_Rear->IsActive())
		{
			LeftTrackKickupFX_Front->SetVectorParameter("WheelSpeed", FVector(LeftWheelSpeedVal));
			LeftTrackKickupFX_Front->SetFloatParameter("WheelSpeed", LeftWheelSpeedVal);

			LeftTrackDebrisFX_Rear->SetVectorParameter("WheelSpeed", FVector(LeftWheelSpeedVal));
			LeftTrackDebrisFX_Rear->SetFloatParameter("WheelSpeed", LeftWheelSpeedVal);
		}
		else
		{
			LeftTrackKickupFX_Front->Activate();
			LeftTrackDebrisFX_Rear->Activate();
		}

		if (LeftTrackKickupFX_Rear->IsActive() || LeftTrackDebrisFX_Front->IsActive())
		{
			LeftTrackKickupFX_Rear->Deactivate();
			LeftTrackDebrisFX_Front->Deactivate();
		}
	}
	else
	{
		LeftTrackKickupFX_Front->Deactivate();
		LeftTrackKickupFX_Rear->Deactivate();
		LeftTrackDebrisFX_Front->Deactivate();
		LeftTrackDebrisFX_Rear->Deactivate();
	}
}

void ATankVehicle::RightTrackParticleFX(float RightWheelSpeedVal)
{
	if (RightWheelSpeedVal > 0.f)
	{
		if (RightTrackKickupFX_Rear->IsActive() && RightTrackDebrisFX_Front->IsActive())
		{
			RightTrackKickupFX_Rear->SetVectorParameter("WheelSpeed", FVector(RightWheelSpeedVal));
			RightTrackKickupFX_Rear->SetFloatParameter("WheelSpeed", RightWheelSpeedVal);

			RightTrackDebrisFX_Front->SetVectorParameter("WheelSpeed", FVector(RightWheelSpeedVal));
			RightTrackDebrisFX_Front->SetFloatParameter("WheelSpeed", RightWheelSpeedVal);
		}
		else
		{
			RightTrackKickupFX_Rear->Activate();
			RightTrackDebrisFX_Front->Activate();
		}

		if (RightTrackKickupFX_Front->IsActive() || RightTrackDebrisFX_Rear->IsActive())
		{
			RightTrackKickupFX_Front->Deactivate();
			RightTrackDebrisFX_Rear->Deactivate();
		}
	}
	else if (RightWheelSpeedVal < 0.f)
	{
		if (RightTrackKickupFX_Front->IsActive() && RightTrackDebrisFX_Rear->IsActive())
		{
			RightTrackKickupFX_Front->SetVectorParameter("WheelSpeed", FVector(RightWheelSpeedVal));
			RightTrackKickupFX_Front->SetFloatParameter("WheelSpeed", RightWheelSpeedVal);

			RightTrackDebrisFX_Rear->SetVectorParameter("WheelSpeed", FVector(RightWheelSpeedVal));
			RightTrackDebrisFX_Rear->SetFloatParameter("WheelSpeed", RightWheelSpeedVal);
		}
		else
		{
			RightTrackKickupFX_Front->Activate();
			RightTrackDebrisFX_Rear->Activate();
		}

		if (RightTrackKickupFX_Rear->IsActive() || RightTrackDebrisFX_Front->IsActive())
		{
			RightTrackKickupFX_Rear->Deactivate();
			RightTrackDebrisFX_Front->Deactivate();
		}
	}
	else
	{
		RightTrackKickupFX_Front->Deactivate();
		RightTrackKickupFX_Rear->Deactivate();
		RightTrackDebrisFX_Front->Deactivate();
		RightTrackDebrisFX_Rear->Deactivate();
	}
}

