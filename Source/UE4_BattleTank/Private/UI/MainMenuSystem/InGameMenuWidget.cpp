// Copyright 2018 Stuart McDonald.

#include "InGameMenuWidget.h"

#include "Components/WidgetSwitcher.h"
#include "Components/Button.h"


bool UInGameMenuWidget::Initialize()
{
	if (!Super::Initialize()) { return false; }

	if (Button_Resume)
	{
		Button_Resume->OnClicked.AddDynamic(this, &UInGameMenuWidget::ResumeGame);
	}

	if (Button_Return && WidgetSwitcher)
	{
		Button_Return->OnClicked.AddDynamic(this, &UInGameMenuWidget::ReturnToPreviousWidget);
		Button_Return->SetVisibility(ESlateVisibility::Hidden);

		if (Button_Controls)
		{
			Button_Controls->OnClicked.AddDynamic(this, &UInGameMenuWidget::ControlMenu);
			if (Button_MouseAndKey)
			{
				Button_MouseAndKey->OnClicked.AddDynamic(this, &UInGameMenuWidget::MouseAndKeyControls);
			}
			if (Button_Gamepad)
			{
				Button_Gamepad->OnClicked.AddDynamic(this, &UInGameMenuWidget::GamepadControls);
			}
		}
	}

	if (!Button_LeaveGame) { return false; }
	Button_LeaveGame->OnClicked.AddDynamic(this, &UInGameMenuWidget::LeaveGame);
	
	return true;
}

void UInGameMenuWidget::ResumeGame()
{
	TearDown();
}

void UInGameMenuWidget::ReturnToPreviousWidget()
{
	if (Panel_Controls == WidgetSwitcher->GetActiveWidget())
	{
		WidgetSwitcher->SetActiveWidget(Panel_Menu);
		Button_Resume->SetKeyboardFocus();
		Button_Return->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		WidgetSwitcher->SetActiveWidget(Panel_Controls);
	}
}

void UInGameMenuWidget::ControlMenu()
{
	if (!WidgetSwitcher) { return; }
	if (!Panel_Controls) { return; }

	WidgetSwitcher->SetActiveWidget(Panel_Controls);
	Button_Return->SetKeyboardFocus();
	Button_Return->SetVisibility(ESlateVisibility::Visible);
}

void UInGameMenuWidget::MouseAndKeyControls()
{
	if (!WidgetSwitcher) { return; }
	if (!Panel_MouseAndKey) { return; }

	WidgetSwitcher->SetActiveWidget(Panel_MouseAndKey);
	Button_Return->SetKeyboardFocus();
}

void UInGameMenuWidget::GamepadControls()
{
	if (!WidgetSwitcher) { return; }
	if (!Panel_Gamepad) { return; }

	WidgetSwitcher->SetActiveWidget(Panel_Gamepad);
	Button_Return->SetKeyboardFocus();
}

void UInGameMenuWidget::LeaveGame()
{
	if (!MenuInterface) { return; }
	MenuInterface->ReturnToMainMenu();
}
