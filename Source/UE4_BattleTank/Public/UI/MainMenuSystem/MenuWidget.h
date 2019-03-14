// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuInterface.h"
#include "MenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE4_BATTLETANK_API UMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	/** Calls interface functions (These functions should be overriden by game instance) */
	IMenuInterface * MenuInterface;
	
public:
	/** Sets input mode */
	void Setup();

	/** Remove widget from veiwport and return input to controller */
	void TearDown();

	/** Sets menu interface variable */
	void SetMenuInterface(IMenuInterface * MenuInt);

protected:
	virtual void OnLevelRemovedFromWorld(ULevel * InLevel, UWorld * InWorld) override;
	
};
