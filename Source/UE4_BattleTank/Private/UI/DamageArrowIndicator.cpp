// Copyright 2018 - 2021 Stuart McDonald.


#include "DamageArrowIndicator.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Kismet/KismetMathLibrary.h"
#include "Animation/WidgetAnimation.h"


void UDamageArrowIndicator::NativeConstruct()
{
	Super::NativeConstruct();
	EndDelegate.BindDynamic(this, &UDamageArrowIndicator::AnimationFinished);

	if (Anim_Indicator)
	{
		BindToAnimationFinished(Anim_Indicator, EndDelegate);
	}
}

void UDamageArrowIndicator::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (EnemyToIndicate)
	{
		GetEnemyDirection();
	}
}

void UDamageArrowIndicator::SetValues(AActor * DamageCauser)
{
	SetIsEnabled(true);
	EnemyToIndicate = DamageCauser;
	PlayAnimationForward(Anim_Indicator);
}

void UDamageArrowIndicator::ResetIndicator()
{
	SetIsEnabled(true);
	PlayAnimationAtTime(Anim_Indicator);
}

void UDamageArrowIndicator::GetEnemyDirection()
{
	if (GetOwningPlayer())
	{
		FVector Loc;
		FRotator Rot;
		GetOwningPlayer()->GetPlayerViewPoint(Loc, Rot);
		FRotator GetRot = UKismetMathLibrary::FindLookAtRotation(Loc, EnemyToIndicate->GetActorLocation());
		float Angle = GetRot.Yaw - Rot.Yaw;
		Panel_Indicator->SetRenderAngle(Angle);
	}
}

void UDamageArrowIndicator::AnimationFinished()
{
	if (Anim_Indicator)
	{
		SetIsEnabled(false);

		// Removed as stops widget ticking when unused
		RemoveFromViewport();
	}
}

