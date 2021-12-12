// Copyright 2018 - 2021 Stuart McDonald.

#include "PlayerWidget.h"
#include "Animation/WidgetAnimation.h"
#include "MovieScene.h"

#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"
#include "Components/Overlay.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/ProgressBar.h"
#include "Player/TankVehicle.h"
#include "Player/AimingComponent.h"
#include "Player/TankPlayerController.h"
#include "DamageArrowIndicator.h"

#define LOCTEXT_NAMESPACE "PlayerHud"

bool UPlayerWidget::Initialize()
{
	if (!Super::Initialize()) { return false; }

	if (Panel_WarningOutOfBounds)
	{
		Panel_WarningOutOfBounds->SetVisibility(ESlateVisibility::Hidden);
	}

	return true;
}

void UPlayerWidget::NativeConstruct()
{
	InitialiseRefs();
	FillAnimationsMap();

	Super::NativeConstruct();
}

void UPlayerWidget::InitialiseRefs()
{
	ATankPlayerController * PC = GetOwningPlayer() ? Cast<ATankPlayerController>(GetOwningPlayer()) : nullptr;
	
	if (PC && PC->GetPawn())
	{
		PlayerPawn = Cast<ATankVehicle>(PC->GetPawn());
		AimCompRef = PC->GetAimCompRef();
	}

	NumberFormat.MinimumIntegralDigits = MinDigits;
}


////////////////////////////////////////////////////////////////////////////////
// Player data display

void UPlayerWidget::SetMaxAmmoDisplay()
{
	if (!AimCompRef) { InitialiseRefs(); }

	if (AimCompRef && Text_MaxAmmo)
	{	
		FText AmmoText = FText::GetEmpty();
		AmmoText = FText::AsNumber(AimCompRef->GetMaxRounds(), &NumberFormat);
		Text_MaxAmmo->SetText(AmmoText);
	}
}

void UPlayerWidget::UpdateWeaponDisplay()
{
	if (!AimCompRef) { InitialiseRefs(); }

	if (Text_AmmoRemaining)
	{
		Text_AmmoRemaining->SetText(GetAmmoText());
	}
	if (Image_Crosshair)
	{
		SetCrosshairColor();
	}
}

FText UPlayerWidget::GetAmmoText() const
{
	FText AmmoText = FText::GetEmpty();

	if (AimCompRef)
	{
		AmmoText = FText::AsNumber(AimCompRef->GetRoundsRemaining(), &NumberFormat);
		return AmmoText;
	}

	return AmmoText;
}

void UPlayerWidget::SetCrosshairColor()
{
	if (AimCompRef)
	{
		if (AimCompRef->GetFiringState() == EFiringState::Reloading)
		{
			PlayAnimationByName("Anim_CrosshairReload", 0.f, 0, EUMGSequencePlayMode::Forward, 1.f);
		}
		else if (AimCompRef->GetFiringState() == EFiringState::OutOfAmmo)
		{
			Image_Crosshair->SetContentColorAndOpacity(FLinearColor(0.f,0.f,0.f,0.4f));
		}
		else
		{
			StopAllAnimations();
		}
	}
}

void UPlayerWidget::UpdateHealthDisplay()
{
	if (!PlayerPawn) { InitialiseRefs(); }
	if (!PlayerPawn) { return; }

	if (Text_HealthRemaining)
	{
		FText HealthText = FText::GetEmpty();
		HealthText = FText::AsNumber(PlayerPawn->GetHealth());
		Text_HealthRemaining->SetText(HealthText);
	}

	if (Bar_HealthRemaining)
	{
		Bar_HealthRemaining->SetPercent(PlayerPawn->GetHealthPercent());

		if (!bLowHealthWarning && PlayerPawn->GetHealth() <= LowHealthThreshold)
		{
			bLowHealthWarning = true;
			Bar_HealthRemaining->SetFillColorAndOpacity(FLinearColor::Red);
		}
		else if (bLowHealthWarning && PlayerPawn->GetHealth() > LowHealthThreshold)
		{
			bLowHealthWarning = false;
			Bar_HealthRemaining->SetFillColorAndOpacity(FLinearColor::White);
		}
	}
}

void UPlayerWidget::AddDamageIndicator(AActor * DamageCauser)
{
	if (DmgIndicatorBlueprint)
	{
		if (ArrowIndicatorMap.Contains(DamageCauser))
		{
			DamageCauserArray.Remove(DamageCauser);
			DamageCauserArray.Insert(DamageCauser, 0);
			if (ArrowIndicatorMap[DamageCauser] && ArrowIndicatorMap[DamageCauser]->GetIsEnabled())
			{		
				ArrowIndicatorMap[DamageCauser]->ResetIndicator();
			}
			else
			{
				UDamageArrowIndicator * DmgIndWidget = Cast<UDamageArrowIndicator>(CreateWidget(this, DmgIndicatorBlueprint));
				if (DmgIndWidget)
				{
					ArrowIndicatorMap.Emplace(DamageCauser, DmgIndWidget);
					Panel_Damage->AddChild(DmgIndWidget);
					DmgIndWidget->SetValues(DamageCauser);
				}
			}
		}
		else if (ArrowIndicatorMap.Num() < MaxArrowIndicators)
		{
			DamageCauserArray.Insert(DamageCauser, 0);
			UDamageArrowIndicator * DmgIndWidget = Cast<UDamageArrowIndicator>(CreateWidget(this, DmgIndicatorBlueprint));

			if (DmgIndWidget)
			{
				ArrowIndicatorMap.Add(DamageCauser, DmgIndWidget);
				Panel_Damage->AddChild(DmgIndWidget);
				DmgIndWidget->SetValues(DamageCauser);
			}
		}
		else if (ArrowIndicatorMap.Contains(DamageCauserArray.Last()))
		{
			UDamageArrowIndicator * DmgIndWidget = Cast<UDamageArrowIndicator>(CreateWidget(this, DmgIndicatorBlueprint));

			if (DmgIndWidget)
			{
				if (ArrowIndicatorMap[DamageCauserArray.Last()]->GetIsEnabled())
				{
					ArrowIndicatorMap[DamageCauserArray.Last()]->RemoveFromViewport();
					ArrowIndicatorMap.Add(DamageCauser, DmgIndWidget);
					Panel_Damage->AddChild(DmgIndWidget);
					DmgIndWidget->SetValues(DamageCauser);
				}
				else
				{
					ArrowIndicatorMap.Add(DamageCauser, DmgIndWidget);
					Panel_Damage->AddChild(DmgIndWidget);
					DmgIndWidget->SetValues(DamageCauser);
				}
			}
			
			ArrowIndicatorMap.Remove(DamageCauserArray.Last());
			DamageCauserArray.RemoveAt(DamageCauserArray.Num() - 1);
			DamageCauserArray.Insert(DamageCauser, 0);
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
// Warning display

void UPlayerWidget::UpdateDamageIndicators(AActor * DamageCauser)
{
	if (!PlayerPawn) { InitialiseRefs(); }
	if (!PlayerPawn) { return; }

	if (Panel_Damage)
	{
		AddDamageIndicator(DamageCauser);
	}
}

void UPlayerWidget::NotifyOutOfAmmo()
{
	if (Text_AmmoRemaining)
	{
		Text_AmmoRemaining->SetColorAndOpacity(FSlateColor(FLinearColor(0.2f, 0.2f, 0.2f)));
	}
	PlayAnimationByName("Anim_WarnOutOfAmmo", 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
}

void UPlayerWidget::RemoveAmmoWarnings()
{
	if (Text_AmmoRemaining)
	{
		Text_AmmoRemaining->SetColorAndOpacity(FSlateColor(FColor::White));
	}
}

void UPlayerWidget::NotifyOutOfMatchArea()
{
	if (Panel_WarningOutOfBounds && !Panel_WarningOutOfBounds->IsVisible())
	{
		Panel_WarningOutOfBounds->SetVisibility(ESlateVisibility::Visible);
		Text_WarningOutOfBounds->SetText(LOCTEXT("Message", "Turn Back!" LINE_TERMINATOR "Leaving Combat Area!" LINE_TERMINATOR "Time Left"));
	}
}

void UPlayerWidget::RemoveCombatAreaWarning()
{
	if (Panel_WarningOutOfBounds && Panel_WarningOutOfBounds->IsVisible())
	{
		Panel_WarningOutOfBounds->SetVisibility(ESlateVisibility::Hidden);
	}
}


////////////////////////////////////////////////////////////////////////////////
// Animations

void UPlayerWidget::FillAnimationsMap()
{
	AnimationsMap.Empty();

	UProperty* Prop = GetClass()->PropertyLink;

	// Run through all properties of this class to find any widget animations
	while (Prop)
	{
		// Only interested in object properties
		if (Prop->GetClass() == UObjectProperty::StaticClass())
		{
			UObjectProperty* ObjProp = Cast<UObjectProperty>(Prop);

			// Only want the properties that are widget animations
			if (ObjProp->PropertyClass == UWidgetAnimation::StaticClass())
			{
				UObject* Obj = ObjProp->GetObjectPropertyValue_InContainer(this);

				UWidgetAnimation* WidgetAnim = Cast<UWidgetAnimation>(Obj);

				if (WidgetAnim && WidgetAnim->MovieScene)
				{
					FName AnimName = WidgetAnim->MovieScene->GetFName();
					AnimationsMap.Add(AnimName, WidgetAnim);
				}
			}
		}

		Prop = Prop->PropertyLinkNext;
	}
}

UWidgetAnimation * UPlayerWidget::GetAnimationByName(FName AnimName) const
{
	UWidgetAnimation * const * WidgetAnim = AnimationsMap.Find(AnimName);
	if (WidgetAnim)
	{
		return *WidgetAnim;
	}
	return nullptr;
}

void UPlayerWidget::PlayAnimationByName(FName AnimName, float StartAtTime, int32 NumLoopsToPlay, EUMGSequencePlayMode::Type PlayMode, float PlaybackSpeed)
{
	UWidgetAnimation * WidgetAnim = GetAnimationByName(AnimName);
	if (WidgetAnim)
	{
		PlayAnimation
		(
			WidgetAnim,
			StartAtTime,
			NumLoopsToPlay,
			PlayMode,
			PlaybackSpeed
		);
	}
}

#undef LOCTEXT_NAMESPACE

