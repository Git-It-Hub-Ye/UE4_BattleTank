// Copyright 2018 - 2021 Stuart McDonald.

#include "TankPlayerController.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"

#include "BattleInstance.h"
#include "Online/BattleTankGameModeBase.h"
#include "UI/BattleHUD.h"
#include "Tank.h"
#include "AimingComponent.h"


ATankPlayerController::ATankPlayerController()
{
	bAllowPawnInput = false;
	bInGameMenuInViewport = false;
}

void ATankPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!GetPawn()) { return; }
	if (!bAllowPawnInput) { GetPawn()->DisableInput(this); }
	TogglePlayerHud(true);
}

void ATankPlayerController::SetPawn(APawn * InPawn)
{
	Super::SetPawn(InPawn);
	/*if (InPawn)
	{
		auto PossessedTank = Cast<ATank>(InPawn);

		if (!ensure(PossessedTank)) { return; }
		PossessedTank->OnDeath.AddUniqueDynamic(this, &ATankPlayerController::OnPossessedTankDeath);
	}*/
}


////////////////////////////////////////////////////////////////////////////////
// Setup

void ATankPlayerController::ClientInGame()
{
	ShowMatchScoreboard();
}

void ATankPlayerController::ClientMatchStarted()
{
	bAllowPawnInput = true;

	if (!GetPawn() || bInGameMenuInViewport) { return; }
	GetPawn()->EnableInput(this);
}

void ATankPlayerController::ClientMatchFinished()
{
	bAllowPawnInput = false;
	TogglePlayerHud(false);
}

void ATankPlayerController::ClientGameEnded()
{
	DisableInput(this);
	RemoveInGameMenu();
	if (GetPawn())
	{
		GetPawn()->DetachFromControllerPendingDestroy();
	}
}

void ATankPlayerController::ClientNotifyOfMatchState()
{
	ABattleHUD * BHUD = Cast<ABattleHUD>(GetPlayerHud());
	if (BHUD)
	{
		BHUD->UpdateMatchStateDisplay();
	}
}

void ATankPlayerController::HandleReturnToMainMenu()
{
	RemoveMatchScoreboard();
	CleanupSessionOnReturnToMenu();
}

void ATankPlayerController::CleanupSessionOnReturnToMenu()
{
	UBattleInstance * GI = GetWorld() ? Cast<UBattleInstance>(GetWorld()->GetGameInstance()) : nullptr;

	if (GI)
	{
		GI->CleanupSessionOnReturnToMenu();
	}
}

void ATankPlayerController::OnPossessedTankDeath()
{
	TogglePlayerHud(false);

	ABattleTankGameModeBase * BTGM = Cast<ABattleTankGameModeBase>(GetWorld()->GetAuthGameMode());
	if (BTGM)
	{
		BTGM->OnPlayerDeath(this);
	}

	if (GetPawn())
	{
		AimCameraAfterDeath(LocationOfEnemy);
	}
	else if (BTGM)
	{
		BTGM->TransitionToMapCamera(this);
	}
}

void ATankPlayerController::AimCameraAfterDeath(FVector LookAtLocation)
{
	FVector CurrentLocation;
	CurrentLocation = GetPawn()->GetActorLocation();
	CurrentLocation.Z += 400.f;

	FRotator GetRot = UKismetMathLibrary::FindLookAtRotation(CurrentLocation, LookAtLocation);
	FRotator CamRot = FRotator(GetRot.Pitch, GetRot.Yaw, 0.f);

	GetPawn()->DetachFromControllerPendingDestroy();
	SetInitialLocationAndRotation(CurrentLocation, CamRot);
}


////////////////////////////////////////////////////////////////////////////////
// Input

void ATankPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction("InGameMenu", IE_Released, this, &ATankPlayerController::ToggleInGameMenu);
	InputComponent->BindAction("Scoreboard", IE_Pressed, this, &ATankPlayerController::ShowLeaderboard);
	InputComponent->BindAction("Scoreboard", IE_Released, this, &ATankPlayerController::HideLeaderboard);
}

void ATankPlayerController::DetermineInput()
{
	bInGameMenuInViewport = false;

	if (bAllowPawnInput)
	{		
		if (!GetPawn()) { return; }
		GetPawn()->EnableInput(this);
	}
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
	UBattleInstance * BGI = GetWorld() ? Cast<UBattleInstance>(GetWorld()->GetGameInstance()) : nullptr;
	if (BGI)
	{
		BGI->ToggleInGameMenu();
		if (BGI->GetIsGameMenuVisible())
		{
			bInGameMenuInViewport = true;

			if (!GetPawn()) { return; }
			GetPawn()->DisableInput(this);
		}
	}
}

void ATankPlayerController::RemoveInGameMenu()
{
	UBattleInstance * BGI = GetWorld() ? Cast<UBattleInstance>(GetWorld()->GetGameInstance()) : nullptr;
	if (BGI && BGI->GetIsGameMenuVisible())
	{
		BGI->ToggleInGameMenu();
	}
}

void ATankPlayerController::ShowLeaderboard()
{
	ABattleHUD * BHUD = Cast<ABattleHUD>(GetPlayerHud());
	if (BHUD)
	{
		BHUD->ShowLeaderboard(true);
	}
}

void ATankPlayerController::HideLeaderboard()
{
	ABattleHUD * BHUD = Cast<ABattleHUD>(GetPlayerHud());
	if (BHUD)
	{
		BHUD->ShowLeaderboard(false);
	}
}

void ATankPlayerController::ShowMatchScoreboard()
{
	ABattleHUD * BHUD = Cast<ABattleHUD>(GetPlayerHud());
	if (BHUD)
	{
		BHUD->ShowScoreboard(true);
	}
}

void ATankPlayerController::RemoveMatchScoreboard()
{
	ABattleHUD * BHUD = Cast<ABattleHUD>(GetPlayerHud());
	if (BHUD)
	{
		BHUD->ShowScoreboard(false);
	}
}


////////////////////////////////////////////////////////////////////////////////
// Controller data

void ATankPlayerController::LookAtLocation(FVector EnemyLocation)
{
	LocationOfEnemy = EnemyLocation;
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

