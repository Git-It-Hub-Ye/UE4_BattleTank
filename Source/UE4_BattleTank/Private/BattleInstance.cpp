// Copyright 2018 Stuart McDonald.

#include "BattleInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "GenericPlatform/GenericPlatformMisc.h"

#include "Blueprint/UserWidget.h"
#include "MainMenuSystem/MenuWidget.h"
#include "Player/TankPlayerController.h"


UBattleInstance::UBattleInstance(const FObjectInitializer & ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> DefaultMainMenuWidget(TEXT("/Game/Dynamic/UI/WBP_MainMenu"));
	if (DefaultMainMenuWidget.Class)
	{
		MainMenuWidget = DefaultMainMenuWidget.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> DefaultInGameMenuWidget(TEXT("/Game/Dynamic/UI/WBP_InGameMenu"));
	if (DefaultInGameMenuWidget.Class)
	{
		InGameMenuWidget = DefaultInGameMenuWidget.Class;
	}
}

void UBattleInstance::init()
{
}

void UBattleInstance::LoadMenu()
{
	if (!MainMenuWidget) { return; }
	UMenuWidget * Menu = CreateWidget<UMenuWidget>(this, MainMenuWidget);

	if (!Menu) { return; }
	// Instance implements menu interface
	Menu->SetMenuInterface(this);
	Menu->Setup();
}

void UBattleInstance::LoadInGameMenu()
{
	if (!InGameMenuWidget) { return; }
	InGameMenu = CreateWidget<UMenuWidget>(this, InGameMenuWidget);

	if (!InGameMenu) { return; }
	// Instance implements menu interface
	InGameMenu->SetMenuInterface(this);
	InGameMenu->Setup();
}

void UBattleInstance::RemoveInGameMenu()
{
	if (!InGameMenu) { return; }
	InGameMenu->TearDown();
}

void UBattleInstance::ToggleInGameMenu()
{
	if (InGameMenu && InGameMenu->IsValidLowLevel() && InGameMenu->IsVisible())
	{
		InGameMenu->TearDown();
	}
	else
	{
		LoadInGameMenu();
	}
}

bool UBattleInstance::GetIsGameMenuVisible() const
{
	return InGameMenu && InGameMenu->IsValidLowLevel() && InGameMenu->IsVisible();
}

void UBattleInstance::CleanupSessionOnReturnToMenu()
{
	ReturnToMainMenu();
}

void UBattleInstance::DeterminePlayerInput()
{
	ATankPlayerController * PC = Cast<ATankPlayerController>(GetFirstLocalPlayerController());
	if (!PC) { return; }

	PC->DetermineInput();
}

void UBattleInstance::HostOffline()
{
	if (!GetWorld()) { return; }
	GetWorld()->ServerTravel("/Game/Dynamic/Levels/BattleGround");
}

void UBattleInstance::HostOnline()
{
	UE_LOG(LogTemp, Warning, TEXT("Host"))
}

void UBattleInstance::JoinOnline()
{
	UE_LOG(LogTemp, Warning, TEXT("Join"))
}

void UBattleInstance::ReturnToMainMenu()
{
	if (!GetWorld()) { return; }
	GetWorld()->ServerTravel("/Game/Dynamic/Levels/MainMenu");
}

void UBattleInstance::OuitGame()
{
	APlayerController * PC = GetFirstLocalPlayerController();
	if (!PC) { return; }

	PC->ConsoleCommand("quit");
}

