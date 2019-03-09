// Copyright 2018 Stuart McDonald.

#include "BattleInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"


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

void UBattleInstance::LoadMainMenu()
{
	if (!MenuWidget) { return; }
	UUserWidget * MainMenu = CreateWidget<UUserWidget>(this, MenuWidget);

	if (!MainMenu) { return; }
	MainMenu->AddToViewport();

	APlayerController * PC = GetFirstLocalPlayerController();
	if (!PC) { return; }

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(MainMenu->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	PC->SetInputMode(InputMode);
	PC->bShowMouseCursor = true;
}

