// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"


class UButton;

UCLASS()
class UE4_BATTLETANK_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UButton * Button_Offline;

	UPROPERTY(meta = (BindWidget))
	UButton * Button_Online;

protected:
	virtual bool Initialize() override;

private:
	UFUNCTION()
	void OfflinePlay();
	
	UFUNCTION()
	void OnlinePlay();


};
