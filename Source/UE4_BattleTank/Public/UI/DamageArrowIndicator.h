// Copyright 2018 - 2022 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageArrowIndicator.generated.h"

class UImage;
class AActor;
class UWidgetAnimation;
class UOverlay;

/**
 * Arrow indicator to point to damage instigator
 */
UCLASS()
class UE4_BATTLETANK_API UDamageArrowIndicator : public UUserWidget
{
	GENERATED_BODY()

protected:
	/** Indicator image */
	UPROPERTY(meta = (BindWidget))
	UImage * Image_Arrow;

	/** Indicator panel */
	UPROPERTY(meta = (BindWidget))
	UOverlay * Panel_Indicator;

private:
	/** Enemy to point indicator towards */
	AActor * EnemyToIndicate;

	/** Sets event to call when animation ends */
	FWidgetAnimationDynamicEvent EndDelegate;

	/** Indicator animation */
	UPROPERTY(meta = (BindWidgetAnim))
	UWidgetAnimation * Anim_Indicator;

public:
	/** Setsup display data and animations when constructed */
	virtual void NativeConstruct() override;

	/** Called every Tick */
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	/** Set values for indicator to track */
	void SetValues(AActor * DamageCauser);

	/** Reset indicator timer */
	void ResetIndicator();

	float GetCurrentTime() { return GetAnimationCurrentTime(Anim_Indicator); }

private:
	/** Gets rotation between viewport and enemy */
	void GetEnemyDirection();

	/** Disable Arrow indicator when animation has finished */
	UFUNCTION()
	void AnimationFinished();
	
};
