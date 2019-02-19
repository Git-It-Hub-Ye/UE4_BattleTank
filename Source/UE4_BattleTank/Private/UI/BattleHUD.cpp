// Copyright 2018 Stuart McDonald.

#include "BattleHUD.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "UI/PlayerWidget.h"
#include "UI/InGameMenuWidget.h"
#include "UI/ScoreboardWidget.h"
#include "UI/MatchDisplayWidget.h"

ABattleHUD::ABattleHUD()
{
	PlayerUI = UPlayerWidget::StaticClass();
	InGameMenu = UInGameMenuWidget::StaticClass();
	ScoreboardUI = UScoreboardWidget::StaticClass();

	static ConstructorHelpers::FClassFinder<UUserWidget> DefaultPlayerUIWidget(TEXT("/Game/Dynamic/UI/WBP_PlayerHud"));
	if (DefaultPlayerUIWidget.Class)
	{
		PlayerUI = DefaultPlayerUIWidget.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> DefaultInGameMenuWidget(TEXT("/Game/Dynamic/UI/WBP_InGameMenu"));
	if (DefaultInGameMenuWidget.Class)
	{
		InGameMenu = DefaultInGameMenuWidget.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> DefaultScoreboardWidget(TEXT("/Game/Dynamic/UI/WBP_Scoreboard"));
	if (DefaultScoreboardWidget.Class)
	{
		ScoreboardUI = DefaultScoreboardWidget.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> DefaultMatchDisplayWidget(TEXT("/Game/Dynamic/UI/WBP_MatchDisplay"));
	if (DefaultMatchDisplayWidget.Class)
	{
		MatchDisplayWidget = DefaultMatchDisplayWidget.Class;
	}
}


////////////////////////////////////////////////////////////////////////////////
// Display widgets

void ABattleHUD::ShowPlayerHud(bool bDisplayThisUI)
{
	DisplayPlayerHud(bDisplayThisUI);
}

void ABattleHUD::ShowInGameMenu(bool bOnGameOver)
{
	DisplayInGameMenu(bOnGameOver);
}

void ABattleHUD::ShowScoreboard(bool bDisplayThisUI)
{
	DisplayScoreboard(bDisplayThisUI);
}

void ABattleHUD::ShowInMatchDisplay(bool bDisplayThisUI)
{
	DisplayInMatchWidget(bDisplayThisUI);
}

void ABattleHUD::RemoveWidgetsOnGameOver()
{
	DisplayInGameMenu(true);
	DisplayScoreboard(false);
	DisplayPlayerHud(false);
}

void ABattleHUD::DisplayPlayerHud(bool bDisplayThisUI)
{
	if (!PlayerOwner) { return; }

	if (bDisplayThisUI)
	{
		PlayerWidget = CreateWidget<UPlayerWidget>(PlayerOwner, PlayerUI);
		if (PlayerWidget && PlayerWidget->IsValidLowLevel() && !PlayerWidget->IsVisible())
		{
			PlayerWidget->InitialiseRefs(PlayerOwner);
			PlayerWidget->AddToViewport();
		}
	}
	else
	{
		if (PlayerWidget && PlayerWidget->IsValidLowLevel() && PlayerWidget->IsVisible())
		{
			PlayerWidget->RemoveFromParent();
		}
	}
}

void ABattleHUD::DisplayInGameMenu(bool bOnGameOver)
{
	if (!PlayerOwner) { return; }

	if (InGameMenuWidget && InGameMenuWidget->IsValidLowLevel() && InGameMenuWidget->IsVisible())
	{
		PlayerOwner->SetInputMode(FInputModeGameOnly());
		InGameMenuWidget->RemoveFromParent();
	}
	else if (!bOnGameOver)
	{
		InGameMenuWidget = CreateWidget<UInGameMenuWidget>(PlayerOwner, InGameMenu);

		if (InGameMenuWidget && InGameMenuWidget->IsValidLowLevel() && !InGameMenuWidget->IsVisible())
		{
			InGameMenuWidget->InitialisePlayerController(PlayerOwner);
			InGameMenuWidget->AddToViewport();
		}
	}
}

void ABattleHUD::DisplayScoreboard(bool bDisplayThisUI)
{
	if (!PlayerOwner) { return; }

	if (bDisplayThisUI)
	{
		ScoreboardWidget = CreateWidget<UScoreboardWidget>(PlayerOwner, ScoreboardUI);
		if (ScoreboardWidget && ScoreboardWidget->IsValidLowLevel() && !ScoreboardWidget->IsVisible())
		{
			ScoreboardWidget->AddToViewport();
		}
	}
	else
	{
		if (ScoreboardWidget && ScoreboardWidget->IsValidLowLevel() && ScoreboardWidget->IsVisible())
		{
			ScoreboardWidget->RemoveFromParent();
		}
	}
}

void ABattleHUD::DisplayInMatchWidget(bool bDisplayThisUI)
{
	if (bDisplayThisUI)
	{
		InMacthDisplay = CreateWidget<UMatchDisplayWidget>(PlayerOwner, MatchDisplayWidget);
		if (InMacthDisplay && InMacthDisplay->IsValidLowLevel() && !InMacthDisplay->IsVisible())
		{
			InMacthDisplay->AddToViewport();
		}
	}
	else
	{
		if (InMacthDisplay && InMacthDisplay->IsValidLowLevel() && InMacthDisplay->IsVisible())
		{
			InMacthDisplay->RemoveFromParent();
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
// Updating hud

void ABattleHUD::UpdateHealthDisplay()
{
	if (PlayerWidget && PlayerWidget->IsValidLowLevel() && PlayerWidget->IsVisible())
	{
		PlayerWidget->AdjustHealthDisplay();
	}
}

void ABattleHUD::UpdateFiringStateDisplay()
{
	if (PlayerWidget && PlayerWidget->IsValidLowLevel() && PlayerWidget->IsVisible())
	{
		PlayerWidget->AdjustFiringDisplay();
	}
}

void ABattleHUD::UpdateScoreboard()
{
	if (ScoreboardWidget && ScoreboardWidget->IsValidLowLevel() && ScoreboardWidget->IsVisible())
	{
		ScoreboardWidget->UpdateData();
	}
}

void ABattleHUD::WarnOfLowAmmo(bool bLowAmmo)
{
	if (PlayerWidget && PlayerWidget->IsValidLowLevel() && PlayerWidget->IsVisible())
	{
		PlayerWidget->NotifyLowAmmo(bLowAmmo);
	}
}

void ABattleHUD::WarnOutOfAmmo(bool bOutOfAmmo)
{
	if (PlayerWidget && PlayerWidget->IsValidLowLevel() && PlayerWidget->IsVisible())
	{
		PlayerWidget->NotifyOutOfAmmo(bOutOfAmmo);
	}
}

void ABattleHUD::WarnOutOfMatchArea(bool bOutOfArea)
{
	if (PlayerWidget && PlayerWidget->IsValidLowLevel() && PlayerWidget->IsVisible())
	{
		PlayerWidget->NotifyOutOfMatchArea(bOutOfArea);
	}
}

void ABattleHUD::UpdateMatchEndDisplay()
{
	UE_LOG(LogTemp, Warning, TEXT("End"))
}

bool ABattleHUD::IsGameMenuInViewport()
{
	return InGameMenuWidget && InGameMenuWidget->IsValidLowLevel() ? InGameMenuWidget->IsVisible() : false;
}

