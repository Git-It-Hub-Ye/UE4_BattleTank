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
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	ATankPlayerController * PC;
	
public:
	void InitialisePlayerController(ATankPlayerController * PlayerCon);

protected:
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ResumeGame();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ExitGame();
};
