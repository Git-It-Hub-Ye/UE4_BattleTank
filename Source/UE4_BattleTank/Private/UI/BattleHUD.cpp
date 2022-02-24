// Copyright 2018 - 2022 Stuart McDonald.

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
			PlayerWidget->AddToViewport();
		}
	}
	else
	{
		if (PlayerWidget && PlayerWidget->IsValidLowLevel() && PlayerWidget->IsVisible())
		{
			PlayerWidget->RemoveFromViewport();
		}
	}
}

void ABattleHUD::DisplayScoreboard(bool bDisplayThisUI)
{
	if (bDisplayThisUI)
	{
		ScoreboardWidget = CreateWidget<UScoreboardWidget>(PlayerOwner, ScoreboardUI);
		if (ScoreboardWidget)
		{
			ScoreboardWidget->AddToViewport();
		}
	}
	else if (ScoreboardWidget)
	{
		ScoreboardWidget->RemoveFromViewport();
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

void ABattleHUD::UpdateCrosshairDisplay(bool bThirdPersonView, bool bZoomDisplay)
{
	if (PlayerWidget && PlayerWidget->IsValidLowLevel() && PlayerWidget->IsVisible())
	{
		PlayerWidget->UpdateCrosshairDisplay(bThirdPersonView, bZoomDisplay);
	}
}

void ABattleHUD::UpdateHealthDisplay()
{
	if (PlayerWidget && PlayerWidget->IsValidLowLevel() && PlayerWidget->IsVisible())
	{
		PlayerWidget->UpdateHealthDisplay();
	}
}

void ABattleHUD::UpdateDamageIndicator(AActor* DamageCauser)
{
	if (PlayerWidget && PlayerWidget->IsValidLowLevel() && PlayerWidget->IsVisible())
	{
		PlayerWidget->UpdateDamageIndicators(DamageCauser);
	}
}

void ABattleHUD::UpdateWeaponStateDisplay()
{
	if (PlayerWidget && PlayerWidget->IsValidLowLevel() && PlayerWidget->IsVisible())
	{
		PlayerWidget->UpdateWeaponDisplay();
	}
}

void ABattleHUD::WarnOutOfAmmo()
{
	if (PlayerWidget && PlayerWidget->IsValidLowLevel() && PlayerWidget->IsVisible())
	{
		PlayerWidget->NotifyOutOfAmmo();
	}
}

void ABattleHUD::SetMaxAmmoDisplay()
{
	if (PlayerWidget && PlayerWidget->IsValidLowLevel() && PlayerWidget->IsVisible())
	{
		PlayerWidget->SetMaxAmmoDisplay();
	}
}

void ABattleHUD::WarnOutOfCombatArea()
{
	if (PlayerWidget && PlayerWidget->IsValidLowLevel() && PlayerWidget->IsVisible())
	{
		PlayerWidget->NotifyOutOfMatchArea();
	}
}

void ABattleHUD::RemoveAmmoWarnings()
{
	if (PlayerWidget && PlayerWidget->IsValidLowLevel() && PlayerWidget->IsVisible())
	{
		PlayerWidget->RemoveAmmoWarnings();
	}
}

void ABattleHUD::RemoveCombatAreaWarnings()
{
	if (PlayerWidget && PlayerWidget->IsValidLowLevel() && PlayerWidget->IsVisible())
	{
		PlayerWidget->RemoveCombatAreaWarning();
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

