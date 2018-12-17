// Copyright 2018 Stuart McDonald.

#include "InGameMenuWidget.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/TankPlayerController.h"


void UInGameMenuWidget::InitialisePlayerController(ATankPlayerController * PlayerCon)
{
	PC = PlayerCon;
}

void UInGameMenuWidget::ResumeGame()
{
	RemoveFromParent();
	if (PC)
	{
		PC->SetInputMode(FInputModeGameOnly());
		PC->bShowMouseCursor = false;
		PC->NotifyMenuRemoved();
	}
}

void UInGameMenuWidget::ExitGame()
{
	RemoveFromParent();
	if (PC)
	{
		UKismetSystemLibrary::QuitGame(GetWorld(), PC, EQuitPreference::Quit);
	}
}
