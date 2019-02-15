// Copyright 2018 Stuart McDonald.

#include "TankPlayerController.h"
#include "Tank.h"
#include "AimingComponent.h"
#include "Engine/World.h"
#include "BattleTankGameModeBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "UI/BattleHUD.h"


void ATankPlayerController::BeginPlay()
{
	Super::BeginPlay();
	ATank * PlayerPawn = Cast<ATank>(GetPawn());
	if (!PlayerPawn) return;
	
	TogglePlayerHud(true);
}

void ATankPlayerController::SetPawn(APawn * InPawn)
{
	Super::SetPawn(InPawn);
	if (InPawn)
	{
		auto PossessedTank = Cast<ATank>(InPawn);

		if (!ensure(PossessedTank)) { return; }
		PossessedTank->OnDeath.AddUniqueDynamic(this, &ATankPlayerController::OnPossessedTankDeath);
	}
}

void ATankPlayerController::EnemyThatKilledPlayer(FVector EnemyLocation)
{
	LocationOfEnemy = EnemyLocation;
}

void ATankPlayerController::OnPossessedTankDeath()
{
	TogglePlayerHud(false);
	AimCameraAfterDeath(GetPawn()->GetActorLocation(), LocationOfEnemy);

	ABattleTankGameModeBase * BTGM = Cast<ABattleTankGameModeBase>(GetWorld()->GetAuthGameMode());

	if (BTGM)
	{
		BTGM->PlayerDestroyed();
	}
}

void ATankPlayerController::AimCameraAfterDeath(FVector CurrentLocation, FVector LookAtLocation)
{
	UnPossess();
	CurrentLocation.Z += 400.f;
	FRotator GetRot = UKismetMathLibrary::FindLookAtRotation(CurrentLocation, LookAtLocation);
	FRotator CamRot = FRotator(GetRot.Pitch, GetRot.Yaw, 0.f);

	SetInitialLocationAndRotation(CurrentLocation, CamRot);
}


////////////////////////////////////////////////////////////////////////////////
// Input

void ATankPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("InGameMenu", IE_Released, this, &ATankPlayerController::ToggleInGameMenu);
	InputComponent->BindAction("Scoreboard", IE_Pressed, this, &ATankPlayerController::ShowScoreboard);
	InputComponent->BindAction("Scoreboard", IE_Released, this, &ATankPlayerController::HideScoreboard);
}

void ATankPlayerController::TogglePlayerHud(bool bShowHud)
{
	ABattleHUD * BHUD = Cast<ABattleHUD>(GetPlayerHud());
	if (BHUD)
	{
		BHUD->ShowPlayerHud(bShowHud);
	}
}

void ATankPlayerController::ToggleInGameMenu()
{
	ABattleHUD * BHUD = Cast<ABattleHUD>(GetPlayerHud());
	if (BHUD)
	{
		BHUD->ShowInGameMenu(false);
	}
}

void ATankPlayerController::ShowScoreboard()
{
	ABattleHUD * BHUD = Cast<ABattleHUD>(GetPlayerHud());
	if (BHUD)
	{
		BHUD->ShowScoreboard(true);
	}
}

void ATankPlayerController::HideScoreboard()
{
	ABattleHUD * BHUD = Cast<ABattleHUD>(GetPlayerHud());
	if (BHUD)
	{
		BHUD->ShowScoreboard(false);
	}
}

bool ATankPlayerController::CanRecieveInput() const
{
	ABattleHUD * BHUD = Cast<ABattleHUD>(GetPlayerHud());
	
	return BHUD ? !BHUD->IsGameMenuInViewport() : true;
}

UAimingComponent * ATankPlayerController::GetAimCompRef() const
{
	if (!GetPawn()) { return nullptr; }
	auto AimingComponent = GetPawn()->FindComponentByClass<UAimingComponent>();
	return AimingComponent;
}

ABattleHUD * ATankPlayerController::GetPlayerHud() const
{
	return Cast<ABattleHUD>(GetHUD());
}


////////////////////////////////////////////////////////////////////////////////
// Weapon usage

FVector ATankPlayerController::GetCrosshairLocation() const
{
	// Find crosshair position in pixel coordinates.
	int32 ViewportSizeX, ViewportSizeY;
	GetViewportSize(ViewportSizeX, ViewportSizeY);
	auto ScreenLocation = FVector2D(ViewportSizeX * CrosshairXLocation, ViewportSizeY * CrosshairYLocation);
	FVector HitLocation;

	return GetSightRayHitLocation(HitLocation);
}

FVector ATankPlayerController::GetSightRayHitLocation(FVector & HitLocation) const
{
	// Find crosshair position in pixel coordinates.
	int32 ViewportSizeX, ViewportSizeY;
	GetViewportSize(ViewportSizeX, ViewportSizeY);
	auto ScreenLocation = FVector2D(ViewportSizeX * CrosshairXLocation, ViewportSizeY * CrosshairYLocation);

	// De-project the screen position of crosshair to a world position.
	FVector LookDirection;

	if (GetLookDirection(ScreenLocation, LookDirection))
	{
		// LineTrace along the look direction and see what is hit (upto a max range).
		return GetLookVectorHitLocation(LookDirection, HitLocation);
	}

	return PlayerCameraManager->GetCameraLocation().ForwardVector;
}

bool ATankPlayerController::GetLookDirection(FVector2D ScreenLocation, FVector & LookDirection) const
{
	FVector CameraWorldLocation; // To be discarded.
	return DeprojectScreenPositionToWorld(
		ScreenLocation.X,
		ScreenLocation.Y,
		CameraWorldLocation,
		LookDirection
	);
}

FVector ATankPlayerController::GetLookVectorHitLocation(FVector LookDirection, FVector & HitLocation) const
{
	FHitResult HitResult;
	auto StartLocation = PlayerCameraManager->GetCameraLocation();
	auto EndLocation = StartLocation + (LookDirection * LineTraceRange);

	GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartLocation,
		EndLocation,
		ECollisionChannel::ECC_Camera
	);
	return HitLocation = HitResult.GetActor() ? HitResult.ImpactPoint : EndLocation;
}

