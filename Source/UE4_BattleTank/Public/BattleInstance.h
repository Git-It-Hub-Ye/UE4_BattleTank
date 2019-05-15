// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MenuInterface.h"
#include "BattleInstance.generated.h"

UCLASS()
class UE4_BATTLETANK_API UBattleInstance : public UGameInstance, public IMenuInterface
{
	GENERATED_BODY()

private:
	/** BP widget for main menu */
	TSubclassOf<class UUserWidget> MainMenuWidget;

	/** BP widget for in game menu */
	TSubclassOf<class UUserWidget> InGameMenuWidget;

	class UMenuWidget * InGameMenu;
	
public:
	UBattleInstance(const FObjectInitializer & ObjectInitializer);

	virtual void init();

	UFUNCTION(BlueprintCallable)
	void LoadMenu();

	void LoadInGameMenu();

	void RemoveInGameMenu();

	void ToggleInGameMenu();

	bool GetIsGameMenuVisible() const;

	void CleanupSessionOnReturnToMenu();
	
protected:
	virtual void DeterminePlayerInput() override;

	virtual void HostOffline() override;

	virtual void HostOnline() override;

	virtual void JoinOnline() override;

	virtual void ReturnToMainMenu() override;

	virtual void OuitGame() override;
	
};
