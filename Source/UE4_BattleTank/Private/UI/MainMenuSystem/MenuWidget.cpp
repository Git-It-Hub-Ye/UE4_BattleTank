// Copyright 2018 to 2021 Stuart McDonald.

#include "MenuWidget.h"


void UMenuWidget::SetMenuInterface(IMenuInterface * MenuInt)
{
	MenuInterface = MenuInt;
}

void UMenuWidget::Setup()
{
	this->AddToViewport();

	APlayerController * PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
	if (!PC) { return; }

	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(this->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	PC->SetInputMode(InputMode);
	PC->bShowMouseCursor = true;
}

void UMenuWidget::TearDown()
{
	this->RemoveFromViewport();

	APlayerController * PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
	if (!PC) { return; }

	PC->SetInputMode(FInputModeGameOnly());
	PC->bShowMouseCursor = false;

	if (MenuInterface)
	{
		MenuInterface->DeterminePlayerInput();
	}
}

void UMenuWidget::OnLevelRemovedFromWorld(ULevel * InLevel, UWorld * InWorld)
{
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);
	TearDown();
}

