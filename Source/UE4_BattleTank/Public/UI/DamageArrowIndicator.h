// Copyright 2018 - 2021 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageArrowIndicator.generated.h"

class UImage;
class AActor;

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
	UImage * Image_Arrow;

private:
	/** Enemy to point indicator towards */
	AActor * EnemyToIndicate;

public:
	/** Setsup display data and animations when constructed */
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	/** Set values for indicator to track */
	void SetValues(AActor * DamageCauser);

	/** Reset indicator timer */
	void ResetIndicator();

private:
	void GetEnemyDirection();
	
};
