// Copyright 2018 Stuart McDonald.

#include "BattleInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "MainMenuSystem/MainMenuWidget.h"


UBattleInstance::UBattleInstance(const FObjectInitializer & ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> DefaultMenuWidget(TEXT("/Game/Dynamic/UI/WBP_MainMenu"));
	if (DefaultMenuWidget.Class)
	{
		MenuWidget = DefaultMenuWidget.Class;
	}
}

void UBattleInstance::init()
{

}

void UBattleInstance::LoadMenu()
{
	if (!MenuWidget) { return; }
	UMainMenuWidget * MainMenu = CreateWidget<UMainMenuWidget>(this, MenuWidget);

	if (!MainMenu) { return; }
	// Instance implements menu interface
	MainMenu->SetMenuInterface(this);
	MainMenu->AddToViewport();

	APlayerController * PC = GetFirstLocalPlayerController();
	if (!PC) { return; }

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(MainMenu->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	PC->SetInputMode(InputMode);
	PC->bShowMouseCursor = true;
}

void UBattleInstance::Host()
{
	UE_LOG(LogTemp, Warning, TEXT("Host"))
}

void UBattleInstance::Join()
{
	UE_LOG(LogTemp, Warning, TEXT("Join"))
}

void UBattleInstance::LoadMainMenu()
{
}

