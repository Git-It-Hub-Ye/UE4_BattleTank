// Copyright 2018 - 2021 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageArrowIndicator.generated.h"

class UImage;

/**
 * Arrow indicator to point to damage instigator
 */
UCLASS()
class UE4_BATTLETANK_API UDamageArrowIndicator : public UUserWidget
{
	GENERATED_BODY()

protected:
	/** Ammo image */
	UPROPERTY(meta = (BindWidget))
	UImage* Image_Arrow;
	
};
