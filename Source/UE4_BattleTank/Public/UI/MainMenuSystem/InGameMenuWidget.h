// Copyright 2018 - 2021 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "MenuWidget.h"
#include "InGameMenuWidget.generated.h"

class UWidgetSwitcher;
class UPanelWidget;
class UButton;

/**
 * Display in game menu
 */
UCLASS()
class UE4_BATTLETANK_API UInGameMenuWidget : public UMenuWidget
{
	GENERATED_BODY()

private:
	////////////////////////////////////////////////////////////////////////////////
	// Inital menu view

	/** Switches between different menu layouts */
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher * WidgetSwitcher;

	/** First panel that will be visible to user */
	UPROPERTY(meta = (BindWidget))
	UPanelWidget * Panel_Menu;

	/** Closes menu */
	UPROPERTY(meta = (BindWidget))
	UButton * Button_Resume;

	/** Open control panel */
	UPROPERTY(meta = (BindWidget))
	UButton * Button_Controls;

	/** Leaves current game */
	UPROPERTY(meta = (BindWidget))
	UButton * Button_LeaveGame;


	////////////////////////////////////////////////////////////////////////////////
	// Controls panel and buttons

	/** Shows control types */
	UPROPERTY(meta = (BindWidget))
	UPanelWidget * Panel_Controls;

	/** Open mouse and keyboard controls panel */
	UPROPERTY(meta = (BindWidget))
	UButton * Button_MouseAndKey;

	/** Open gamepad controls panel */
	UPROPERTY(meta = (BindWidget))
	UButton * Button_Gamepad;

	/** Return to previous panel */
	UPROPERTY(meta = (BindWidget))
	UButton * Button_Return;


	////////////////////////////////////////////////////////////////////////////////
	// Controls

	/** Shows mouse and keyboard bindings */
	UPROPERTY(meta = (BindWidget))
	UPanelWidget * Panel_MouseAndKey;

	/** Shows gamepad bindings */
	UPROPERTY(meta = (BindWidget))
	UPanelWidget * Panel_Gamepad;
	
protected:
	virtual bool Initialize() override;

private:
	/** Removes widget and reset input mode */
	UFUNCTION()
	void ResumeGame();

	/** Return to prvious panel */
	UFUNCTION()
	void ReturnToPreviousWidget();

	/** Closes down game */
	UFUNCTION()
	void ControlMenu();

	/** Closes down game */
	UFUNCTION()
	void MouseAndKeyControls();

	/** Closes down game */
	UFUNCTION()
	void GamepadControls();

	/** Leaves game */
	UFUNCTION()
	void LeaveGame();
};
