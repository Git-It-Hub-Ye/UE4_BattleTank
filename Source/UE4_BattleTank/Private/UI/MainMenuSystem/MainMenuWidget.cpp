// Copyright 2018 Stuart McDonald.

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
			Button_Host->OnClicked.AddDynamic(this, &UMainMenuWidget::HostGame);
		}
		if (Button_Join)
		{
			Button_Join->OnClicked.AddDynamic(this, &UMainMenuWidget::JoinGame);
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

void UMainMenuWidget::SetMenuInterface(IMenuInterface * MenuInt)
{
	MenuInterface = MenuInt;
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
	}
}

void UMainMenuWidget::HostGame()
{
	if(!WidgetSwitcher) { return; }
	if (!HostMenu) { return; }

	WidgetSwitcher->SetActiveWidget(HostMenu);
}

void UMainMenuWidget::JoinGame()
{
	if(!WidgetSwitcher) { return; }
	if (!JoinMenu) { return; }

	WidgetSwitcher->SetActiveWidget(JoinMenu);
}

void UMainMenuWidget::StartOfflineGame()
{

}

void UMainMenuWidget::Host()
{
	if (MenuInterface)
	{
		MenuInterface->Host();
	}
}

void UMainMenuWidget::Join()
{
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

void UMainMenuWidget::QuitGame()
{
}
