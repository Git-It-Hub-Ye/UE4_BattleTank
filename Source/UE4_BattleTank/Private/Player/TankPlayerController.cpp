// Copyright 2018 Stuart McDonald.

#include "TankPlayerController.h"
#include "Tank.h"
#include "AimingComponent.h"
#include "Engine/World.h"
#include "BattleTankGameModeBase.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "UI/PlayerWidget.h"
#include "UI/InGameMenuWidget.h"
#include "UI/ScoreboardWidget.h"
#include "Camera/CameraActor.h"
#include "Kismet/KismetMathLibrary.h"

ATankPlayerController::ATankPlayerController()
{
	ConstructorHelpers::FClassFinder<UUserWidget> DefaultPlayerUIWidget(TEXT("/Game/Dynamic/UI/WBP_PlayerHud"));
	if (DefaultPlayerUIWidget.Class)
	{
		PlayerUI = DefaultPlayerUIWidget.Class;
	}

	ConstructorHelpers::FClassFinder<UUserWidget> DefaultInGameMenuWidget(TEXT("/Game/Dynamic/UI/WBP_InGameMenu"));
	if (DefaultInGameMenuWidget.Class)
	{
		InGameMenu = DefaultInGameMenuWidget.Class;
	}

	ConstructorHelpers::FClassFinder<UUserWidget> DefaultScoreboardWidget(TEXT("/Game/Dynamic/UI/WBP_Scoreboard"));
	if (DefaultScoreboardWidget.Class)
	{
		ScoreboardUI = DefaultScoreboardWidget.Class;
	}

	bLookAtInGameMenu = false;
	bLookAtScoreboard = false;
}

void ATankPlayerController::BeginPlay()
{
	Super::BeginPlay();
	ATank * PlayerPawn = Cast<ATank>(GetPawn());
	if (!PlayerPawn) return;

	auto AimingComponent = GetPawn()->FindComponentByClass<UAimingComponent>();
	if (!ensure(AimingComponent)) { return; }

	PlayerWidget = CreateWidget<UPlayerWidget>(this, PlayerUI);
	if (!PlayerWidget) { return; }

	PlayerWidget->InitialiseAimingComp(AimingComponent, PlayerPawn);
	PlayerWidget->AddToViewport();
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
	ABattleTankGameModeBase * BTGM = Cast<ABattleTankGameModeBase>(GetWorld()->GetAuthGameMode());

	if (BTGM)
	{
		BTGM->PlayerDestroyed();
	}

	AimCameraAfterDeath(GetPawn()->GetActorLocation(), LocationOfEnemy);
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

void ATankPlayerController::ToggleInGameMenu()
{
	if (bLookAtInGameMenu)
	{
		HideInGameMenu();
	}
	else
	{
		ShowInGameMenu();
	}
}

void ATankPlayerController::ShowScoreboard()
{
	ScoreboardWidget = CreateWidget<UScoreboardWidget>(this, ScoreboardUI);
	if (ScoreboardWidget->IsValidLowLevel())
	{
		if (!ScoreboardWidget->IsVisible())
		{
			ScoreboardWidget->AddToViewport();
			bLookAtScoreboard = true;
		}
	}
}

void ATankPlayerController::HideScoreboard()
{
	if (ScoreboardWidget->IsValidLowLevel())
	{
		if (ScoreboardWidget->IsVisible())
		{
			ScoreboardWidget->RemoveFromParent();
			bLookAtScoreboard = false;
		}
	}
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


////////////////////////////////////////////////////////////////////////////////
// UI

void ATankPlayerController::ShowInGameMenu()
{
	InGameMenuWidget = CreateWidget<UInGameMenuWidget>(this, InGameMenu);

	if (InGameMenuWidget->IsValidLowLevel())
	{
		InGameMenuWidget->InitialisePlayerController(this);
		InGameMenuWidget->AddToViewport();
		bShowMouseCursor = true;
		bLookAtInGameMenu = true;
	}
}

void ATankPlayerController::HideInGameMenu()
{
	if (InGameMenuWidget->IsValidLowLevel())
	{

		SetInputMode(FInputModeGameOnly());
		InGameMenuWidget->RemoveFromParent();
		bShowMouseCursor = false;
		bLookAtInGameMenu = false;
	}
}

void ATankPlayerController::NotifyMenuRemoved()
{
	bLookAtInGameMenu = false;
}

void ATankPlayerController::UpdateFiringStateDisplay()
{
	if (PlayerWidget)
	{
		PlayerWidget->AdjustFiringDisplay();
	}
}

void ATankPlayerController::UpdateHealthDisplay()
{
	if (PlayerWidget)
	{
		PlayerWidget->AdjustHealthDisplay();
	}
}

void ATankPlayerController::WarnOfLowAmmo(bool bLowAmmo)
{
	if (PlayerWidget)
	{
		PlayerWidget->NotifyLowAmmo(bLowAmmo);
	}
}

void ATankPlayerController::WarnOutOfAmmo(bool bOutOfAmmo)
{
	if (PlayerWidget)
	{
		PlayerWidget->NotifyOutOfAmmo(bOutOfAmmo);
	}
}

void ATankPlayerController::WarnOutOfMatchArea(bool bOutOfArea)
{
	if (PlayerWidget)
	{
		PlayerWidget->NotifyOutOfMatchArea(bOutOfArea);
	}
}

