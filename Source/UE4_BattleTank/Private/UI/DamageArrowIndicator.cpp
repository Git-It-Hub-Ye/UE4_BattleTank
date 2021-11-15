// Copyright 2018 - 2021 Stuart McDonald.


#include "DamageArrowIndicator.h"
#include "Components/Image.h"
#include "Kismet/KismetMathLibrary.h"


void UDamageArrowIndicator::NativeConstruct()
{
	Super::NativeConstruct();
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
	EnemyToIndicate = DamageCauser;
}

void UDamageArrowIndicator::ResetIndicator()
{
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
		Image_Arrow->SetRenderAngle(Angle);
	}
}

