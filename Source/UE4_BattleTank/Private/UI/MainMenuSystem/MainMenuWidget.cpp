// Copyright 2018 to 2021 Stuart McDonald.

#include "MainMenuWidget.h"

#include "Components/WidgetSwitcher.h"
#include "Components/Button.h"


bool UMainMenuWidget::Initialize()
{
	if (!Super::Initialize()) { return false; }
	bPlayOnlineMode = false;
		
	if (Button_Offline)
	{
		Button_Offline->OnClicked.AddDynamic(this, &UMainMenuWidget::OfflinePlay);

		if (Button_PlayOffline)
		{
			Button_PlayOffline->OnClicked.AddDynamic(this, &UMainMenuWidget::StartOfflineGame);
		}
	}
	else if (Button_Online) { return false; }

	if (Button_Online)
	{
		Button_Online->OnClicked.AddDynamic(this, &UMainMenuWidget::OnlinePlay);

		if (Button_Host)
		{
			Button_Host->OnClicked.AddDynamic(this, &UMainMenuWidget::HostGameMenu);
		}
		if (Button_Join)
		{
			Button_Join->OnClicked.AddDynamic(this, &UMainMenuWidget::JoinGameMenu);
		}
	}

	if (Button_Return)
	{
		Button_Return->OnClicked.AddDynamic(this, &UMainMenuWidget::ReturnToPreviousWidget);
	}
	if (Button_Quit)
	{
		Button_Quit->OnClicked.AddDynamic(this, &UMainMenuWidget::QuitGame);
	}
	return true;
}

void UMainMenuWidget::ReturnToPreviousWidget()
{
	if (!WidgetSwitcher || !WidgetSwitcher_Button) { return; }
	if (bPlayOnlineMode)
	{
		if (OnlineMenu == WidgetSwitcher->GetActiveWidget())
		{
			WidgetSwitcher->SetActiveWidget(MainMenu);
			if (Button_Quit)
			{
				WidgetSwitcher_Button->SetActiveWidget(Button_Quit);
			}
		}
		else if (JoinMenu == WidgetSwitcher->GetActiveWidget())
		{
			WidgetSwitcher->SetActiveWidget(OnlineMenu);
		}
		else if (HostMenu == WidgetSwitcher->GetActiveWidget())
		{
			WidgetSwitcher->SetActiveWidget(OnlineMenu);
		}
	}
	else
	{
		WidgetSwitcher->SetActiveWidget(MainMenu);
		if (Button_Quit)
		{
			WidgetSwitcher_Button->SetActiveWidget(Button_Quit);
		}
	}
}

void UMainMenuWidget::OfflinePlay()
{
	if (!WidgetSwitcher) { return; }
	if (!OfflineMenu) { return; }

	bPlayOnlineMode = false;
	WidgetSwitcher->SetActiveWidget(OfflineMenu);

	if (WidgetSwitcher_Button && Button_Return)
	{
		WidgetSwitcher_Button->SetActiveWidget(Button_Return);
		Button_Return->SetKeyboardFocus();
	}
}

void UMainMenuWidget::OnlinePlay()
{
	if (!WidgetSwitcher) { return; }
	if (!OnlineMenu) { return; }

	bPlayOnlineMode = true;
	WidgetSwitcher->SetActiveWidget(OnlineMenu);

	if (WidgetSwitcher_Button && Button_Return)
	{
		WidgetSwitcher_Button->SetActiveWidget(Button_Return);
		Button_Return->SetKeyboardFocus();
	}
}

void UMainMenuWidget::HostGameMenu()
{
	if(!WidgetSwitcher) { return; }
	if (!HostMenu) { return; }

	WidgetSwitcher->SetActiveWidget(HostMenu);

	if (Button_Return)
	{
		Button_Return->SetKeyboardFocus();
	}
}

void UMainMenuWidget::JoinGameMenu()
{
	if(!WidgetSwitcher) { return; }
	if (!JoinMenu) { return; }

	WidgetSwitcher->SetActiveWidget(JoinMenu);

	if (Button_Return)
	{
		Button_Return->SetKeyboardFocus();
	}
}

void UMainMenuWidget::StartOfflineGame()
{
	if (MenuInterface)
	{
		MenuInterface->HostOffline();
	}
}

void UMainMenuWidget::HostGame()
{
	if (MenuInterface)
	{
		MenuInterface->HostOnline();
	}
}

void UMainMenuWidget::JoinGame()
{
	if (MenuInterface)
	{
		MenuInterface->JoinOnline();
	}
}

void UMainMenuWidget::QuitGame()
{
	if (MenuInterface)
	{
		MenuInterface->OuitGame();
	}
}

