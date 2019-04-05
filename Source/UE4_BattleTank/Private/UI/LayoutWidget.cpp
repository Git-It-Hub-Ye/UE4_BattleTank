// Copyright 2018 Stuart McDonald.

#include "LayoutWidget.h"
#include "Components/TextBlock.h"


void ULayoutWidget::SetPlayerName(FString PlayerName)
{
	Text_PlayerName->SetText(FText::FromString(PlayerName));
}

void ULayoutWidget::SetPlayerScore(int32 Score)
{
	Text_Score->SetText(FText::AsNumber(Score));
}

void ULayoutWidget::SetPlayerKills(int32 Kills)
{
	Text_Kills->SetText(FText::AsNumber(Kills));
}

void ULayoutWidget::SetPlayerAssists(int32 Assists)
{
	Text_Assists->SetText(FText::AsNumber(Assists));
}

void ULayoutWidget::SetPlayerDeaths(int32 Deaths)
{
	Text_Deaths->SetText(FText::AsNumber(Deaths));
}


