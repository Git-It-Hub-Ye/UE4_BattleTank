// Copyright 2018 - 2021 Stuart McDonald.

#include "PlayerWidget.h"
#include "Animation/WidgetAnimation.h"
#include "MovieScene.h"

#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Player/Tank.h"
#include "Player/AimingComponent.h"
#include "Player/TankPlayerController.h"

#define LOCTEXT_NAMESPACE "PlayerHud"

bool UPlayerWidget::Initialize()
{
	if (!Super::Initialize()) { return false; }

	if (Text_WarningMessage)
	{
		Text_WarningMessage->SetVisibility(ESlateVisibility::Hidden);
	}

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
		PlayerPawn = Cast<ATank>(PC->GetPawn());
		AimCompRef = PC->GetAimCompRef();
	}
}


////////////////////////////////////////////////////////////////////////////////
// Player data display

void UPlayerWidget::UpdateWeaponDisplay()
{
	if (!AimCompRef) { InitialiseRefs(); }

	if (Text_AmmoRemaining)
	{
		Text_AmmoRemaining->SetText(GetAmmoText());
	}
	if (Image_Crosshair)
	{
		Image_Crosshair->SetColorAndOpacity(GetCrosshairColor());
	}
}

FText UPlayerWidget::GetAmmoText() const
{
	FText AmmoText = FText::GetEmpty();

	if (AimCompRef)
	{
		AmmoText = FText::AsNumber(AimCompRef->GetRoundsRemaining());
		return AmmoText;
	}

	return AmmoText;
}

FColor UPlayerWidget::GetCrosshairColor() const
{
	FColor NewColor = FColor::White;

	if (AimCompRef)
	{
		if (AimCompRef->GetFiringState() == EFiringState::Reloading)
		{
			return NewColor = FColor::Orange;
		}
		else if (AimCompRef->GetFiringState() == EFiringState::OutOfAmmo)
		{
			return NewColor = FColor::Red;
		}
	}
	return NewColor;
}

void UPlayerWidget::UpdateHealthDisplay()
{
	if (!PlayerPawn) { InitialiseRefs(); }
	if (Bar_ArmourRemaining)
	{
		Bar_ArmourRemaining->SetPercent(GetArmourPercent());
	}
	if (Bar_HealthRemaining)
	{
		Bar_HealthRemaining->SetPercent(GetHealthPercent());
	}
}

float UPlayerWidget::GetArmourPercent() const
{
	if (PlayerPawn)
	{
		return PlayerPawn->GetArmourPercent();
	}
	return 0.0f;
}

float UPlayerWidget::GetHealthPercent() const
{
	if (PlayerPawn)
	{
		return PlayerPawn->GetHealthPercent();
	}
	return 0.0f;
}


////////////////////////////////////////////////////////////////////////////////
// Warning display

void UPlayerWidget::NotifyLowAmmo()
{
	if (Text_WarningMessage)
	{
		Text_WarningMessage->SetText(LOCTEXT("Message", "Low Ammo"));
		Text_WarningMessage->SetVisibility(ESlateVisibility::Visible);
		StopAllAnimations();
		PlayAnimationByName("Anim_WarnLowAmmo", 0.f, 0, EUMGSequencePlayMode::PingPong, 1.f);
	}
}

void UPlayerWidget::NotifyOutOfAmmo()
{
	if (Text_WarningMessage)
	{
		Text_WarningMessage->SetText(LOCTEXT("Message", "Out Of Ammo"));
		Text_WarningMessage->SetVisibility(ESlateVisibility::Visible);
		StopAllAnimations();
		PlayAnimationByName("Anim_WarnOutOfAmmo", 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
	}
}

void UPlayerWidget::RemoveAmmoWarnings()
{
	if (Text_WarningMessage)
	{
		Text_WarningMessage->SetVisibility(ESlateVisibility::Hidden);
		StopAllAnimations();
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

