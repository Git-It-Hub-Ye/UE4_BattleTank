// Copyright 2018 Stuart McDonald.

#include "BattleHUD.h"
#include "UObject/ConstructorHelpers.h"

#include "Blueprint/UserWidget.h"
#include "UI/PlayerWidget.h"
#include "UI/ScoreboardWidget.h"


ABattleHUD::ABattleHUD()
{
	PlayerUI = UPlayerWidget::StaticClass();
	ScoreboardUI = UScoreboardWidget::StaticClass();

	static ConstructorHelpers::FClassFinder<UUserWidget> DefaultPlayerUIWidget(TEXT("/Game/Dynamic/UI/WBP_PlayerHud"));
	if (DefaultPlayerUIWidget.Class)
	{
		PlayerUI = DefaultPlayerUIWidget.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> DefaultScoreboardWidget(TEXT("/Game/Dynamic/UI/WBP_MatchScoreboard"));
	if (DefaultScoreboardWidget.Class)
	{
		ScoreboardUI = DefaultScoreboardWidget.Class;
	}
}


////////////////////////////////////////////////////////////////////////////////
// Display widgets

void ABattleHUD::ShowPlayerHud(bool bDisplayThisUI)
{
	DisplayPlayerHud(bDisplayThisUI);
}

void ABattleHUD::ShowScoreboard(bool bDisplayThisUI)
{
	DisplayScoreboard(bDisplayThisUI);
}

void ABattleHUD::ShowLeaderboard(bool bDisplayThisUI)
{
	DisplayLeaderboard(bDisplayThisUI);
}

void ABattleHUD::DisplayPlayerHud(bool bDisplayThisUI)
{
	if (!PlayerOwner) { return; }

	if (bDisplayThisUI)
	{
		PlayerWidget = CreateWidget<UPlayerWidget>(PlayerOwner, PlayerUI);
		if (PlayerWidget && PlayerWidget->IsValidLowLevel() && !PlayerWidget->IsVisible())
		{
			PlayerWidget->InitialiseRefs();
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

void ABattleHUD::DisplayScoreboard(bool bDisplayThisUI)
{
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

void ABattleHUD::DisplayLeaderboard(bool bDisplayThisUI)
{
	if (!PlayerOwner) { return; }

	if (bDisplayThisUI)
	{
		if (ScoreboardWidget && ScoreboardWidget->IsValidLowLevel() && ScoreboardWidget->IsVisible())
		{
			ScoreboardWidget->ShowLeaderboard();
		}
	}
	else
	{
		if (ScoreboardWidget && ScoreboardWidget->IsValidLowLevel() && ScoreboardWidget->IsVisible())
		{
			ScoreboardWidget->HideLeaderboard();
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
// Updating player hud

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


////////////////////////////////////////////////////////////////////////////////
// Updating match hud

void ABattleHUD::UpdateMatchStateDisplay()
{
	if (ScoreboardWidget && ScoreboardWidget->IsValidLowLevel() && ScoreboardWidget->IsVisible())
	{
		ScoreboardWidget->UpdateMatchStateDisplay();
	}
}

void ABattleHUD::UpdateMatchScores()
{
	if (ScoreboardWidget && ScoreboardWidget->IsValidLowLevel() && ScoreboardWidget->IsVisible())
	{
		ScoreboardWidget->UpdateMatchScore();
	}
}

