// Fill out your copyright notice in the Description page of Project Settings.


#include "UEPractice/UEPractice.h"
#include "Course/UPWorldUserWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/SizeBox.h"

void UUPWorldUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!IsValid(AttachedActor))
	{
		RemoveFromParent();

		UE_LOGFMT(LogGame, Warning, "AttachedActor no longer valid, removing Health Widget.");

		return;
	}

	FVector2D WidgetPosition;
	const bool bIsOnScreen = UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(GetOwningPlayer(), AttachedActor->GetActorLocation() + WorldOffset, WidgetPosition, false);

	if (bIsOnScreen)
	{
		if (ParentSizeBox)
		{
			ParentSizeBox->SetRenderTranslation(WidgetPosition);
		}
	}

	if (ParentSizeBox)
	{
		ParentSizeBox->SetVisibility(bIsOnScreen ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
}
