// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameMenuWidget.generated.h"

class ATankPlayerController;

/**
 * Display in game menu
 */
UCLASS()
class UE4_BATTLETANK_API UInGameMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	/** Owning PC */
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	APlayerController * PC;
	
public:
	/** Initialises the PC variable */
	void InitialisePlayerController(APlayerController * PlayerCon);

protected:
	/** Removes widget and reset input mode */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ResumeGame();

	/** Closes down game */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ExitGame();
};
