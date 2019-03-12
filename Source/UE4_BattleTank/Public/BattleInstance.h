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
	TSubclassOf<class UUserWidget> MenuWidget;
	
	
public:
	UBattleInstance(const FObjectInitializer & ObjectInitializer);

	virtual void init();

	UFUNCTION(BlueprintCallable)
	void LoadMenu();
	
protected:
	virtual void Host() override;

	virtual void Join() override;

	virtual void LoadMainMenu() override;
	
};
