// Copyright 2018 to 2021 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "MenuWidget.h"
#include "MainMenuWidget.generated.h"


class UButton;
class UPanelWidget;
class UWidgetSwitcher;

UCLASS()
class UE4_BATTLETANK_API UMainMenuWidget : public UMenuWidget
{
	GENERATED_BODY()
	
private:

	////////////////////////////////////////////////////////////////////////////////
	// Inital menu view

	/** Switches between different menu layouts */
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher * WidgetSwitcher;

	/** Switches between different buttons layouts */
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher * WidgetSwitcher_Button;

	/** First panel that will be visible to user */
	UPROPERTY(meta = (BindWidget))
	UPanelWidget * MainMenu;

	/** Allows user to view offline games */
	UPROPERTY(meta = (BindWidget))
	UButton * Button_Offline;

	/** Allows user to view online games */
	UPROPERTY(meta = (BindWidget))
	UButton * Button_Online;

	/** Allows user return to previous menu */
	UPROPERTY(meta = (BindWidget))
	UButton * Button_Return;

	/** Allows user quit game */
	UPROPERTY(meta = (BindWidget))
	UButton * Button_Quit;


	////////////////////////////////////////////////////////////////////////////////
	// Offline panel and buttons

	/** Panel contains offline games */
	UPROPERTY(meta = (BindWidget))
	UPanelWidget * OfflineMenu;

	/** Allows user to start this game offline */
	UPROPERTY(meta = (BindWidget))
	UButton * Button_PlayOffline;


	////////////////////////////////////////////////////////////////////////////////
	// Online panel and buttons

	/** Panel contains online games */
	UPROPERTY(meta = (BindWidget))
	UPanelWidget * OnlineMenu;

	/** Shows user game types to host */
	UPROPERTY(meta = (BindWidget))
	UPanelWidget * HostMenu;

	/** Allows user to join a game */
	UPROPERTY(meta = (BindWidget))
	UPanelWidget * JoinMenu;

	/** Allows user to host game */
	UPROPERTY(meta = (BindWidget))
	UButton * Button_Host;

	/** Allows user to join game */
	UPROPERTY(meta = (BindWidget))
	UButton * Button_Join;

private:
	bool bPlayOnlineMode;

protected:
	virtual bool Initialize() override;

private:
	/** Returns to previous widget panel */
	UFUNCTION()
	void ReturnToPreviousWidget();

	/** Allows user to view off line games */
	UFUNCTION()
	void OfflinePlay();
	
	/** Gives user the choice to host or join a game */
	UFUNCTION()
	void OnlinePlay();

	/** Allows user to host a game */
	UFUNCTION()
	void HostGameMenu();

	/** Allows user to join a game */
	UFUNCTION()
	void JoinGameMenu();

	/** Creates offline game */
	UFUNCTION()
	void StartOfflineGame();

	/** Creates a lobby for online game */
	UFUNCTION()
	void HostGame();

	/** Joins a game */
	UFUNCTION()
	void JoinGame();

	/** Returns to previous widget panel */
	UFUNCTION()
	void QuitGame();

};
