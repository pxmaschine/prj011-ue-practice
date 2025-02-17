// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPWorldUserWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/SizeBox.h"

void UUPWorldUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!IsValid(AttachedActor))
	{
		RemoveFromParent();

		UE_LOG(LogTemp, Warning, TEXT("AttachedActor no longer valid, removing health widget."));

		return;
	}

	FVector2D WidgetPosition;
	if (UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(GetOwningPlayer(), AttachedActor->GetActorLocation() + WorldOffset, WidgetPosition, false))
	{
		if (ParentSizeBox)
		{
			ParentSizeBox->SetRenderTranslation(WidgetPosition);
		}
	}
}
