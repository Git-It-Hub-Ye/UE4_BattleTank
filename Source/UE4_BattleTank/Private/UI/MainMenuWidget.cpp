// Copyright 2018 Stuart McDonald.

#include "MainMenuWidget.h"
#include "Components/Button.h"


bool UMainMenuWidget::Initialize()
{
	if (!Super::Initialize()) { return false; }

	if (Button_Offline)
	{
		Button_Offline->OnClicked.AddDynamic(this, &UMainMenuWidget::OfflinePlay);
	}
	else if (!Button_Online) { return false; }

	if (Button_Online)
	{
		Button_Online->OnClicked.AddDynamic(this, &UMainMenuWidget::OnlinePlay);
	}
	
	return true;
}

void UMainMenuWidget::OfflinePlay()
{
	UE_LOG(LogTemp, Warning, TEXT("Offline"))
}

void UMainMenuWidget::OnlinePlay()
{
	UE_LOG(LogTemp, Warning, TEXT("Online"))
}
