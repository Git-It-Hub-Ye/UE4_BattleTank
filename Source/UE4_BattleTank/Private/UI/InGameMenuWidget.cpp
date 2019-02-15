// Copyright 2018 Stuart McDonald.

#include "InGameMenuWidget.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/PlayerController.h"


void UInGameMenuWidget::InitialisePlayerController(APlayerController * PlayerCon)
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
