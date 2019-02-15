// Copyright 2018 Stuart McDonald.

#include "BattleHUD.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "UI/PlayerWidget.h"
#include "UI/InGameMenuWidget.h"
#include "UI/ScoreboardWidget.h"

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
}

void ABattleHUD::ShowPlayerHud(bool DisplayThisUI, UAimingComponent * AimingComp, ATank * Pawn)
{
	DisplayPlayerHud(DisplayThisUI, AimingComp, Pawn);
}

void ABattleHUD::ShowInGameMenu()
{
	DisplayInGameMenu();
}

void ABattleHUD::ShowScoreboard(bool DisplayThisUI)
{
	DisplayScoreboard(DisplayThisUI);
}

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

void ABattleHUD::DisplayPlayerHud(bool DisplayThisUI, UAimingComponent * AimingComp, ATank * Pawn)
{
	if (!PlayerOwner) { return; }

	if (DisplayThisUI)
	{
		PlayerWidget = CreateWidget<UPlayerWidget>(PlayerOwner, PlayerUI);
		if (PlayerWidget && PlayerWidget->IsValidLowLevel() && !PlayerWidget->IsVisible())
		{
			PlayerWidget->InitialiseAimingComp(AimingComp, Pawn);
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

void ABattleHUD::DisplayInGameMenu()
{
	if (!PlayerOwner) { return; }

	if (InGameMenuWidget && InGameMenuWidget->IsValidLowLevel() && InGameMenuWidget->IsVisible())
	{
		PlayerOwner->SetInputMode(FInputModeGameOnly());
		InGameMenuWidget->RemoveFromParent();
	}
	else
	{
		InGameMenuWidget = CreateWidget<UInGameMenuWidget>(PlayerOwner, InGameMenu);

		if (InGameMenuWidget && InGameMenuWidget->IsValidLowLevel() && !InGameMenuWidget->IsVisible())
		{
			InGameMenuWidget->InitialisePlayerController(PlayerOwner);
			InGameMenuWidget->AddToViewport();
		}
	}
}

void ABattleHUD::DisplayScoreboard(bool DisplayThisUI)
{
	if (!PlayerOwner) { return; }
	
	if (DisplayThisUI)
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

void ABattleHUD::UpdateScoreboard()
{
	if (ScoreboardWidget && ScoreboardWidget->IsValidLowLevel() && ScoreboardWidget->IsVisible())
	{
		ScoreboardWidget->UpdateData();
	}
}

bool ABattleHUD::IsGameMenuInViewport()
{
	return InGameMenuWidget && InGameMenuWidget->IsValidLowLevel() ? InGameMenuWidget->IsVisible() : false;
}

