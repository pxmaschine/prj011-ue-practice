// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPWorldUserWidget.h"
#include "UEPractice/UEPractice.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/Overlay.h"
#include "Components/CanvasPanelSlot.h"

void UUPWorldUserWidget::AddToRootCanvasPanel(UUserWidget* InNewWidget)
{
	// Grab the 'main hud' which will have a Canvas Panel to use.
	TArray<UUserWidget*> Widgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(InNewWidget, Widgets, UUserWidget::StaticClass(), true);
	// Expect only one main hud, other menus etc. might mess with this and requires slight change (eg. adding a pause menu to the root)
	//check(Widgets.Num() == 1)

	// We might be testing without a HUD...
	if (Widgets.Num() > 0)
	{
		const UUserWidget* MainHUD = Widgets[0];
		UCanvasPanel* CanvasPanel = Cast<UCanvasPanel>(MainHUD->GetRootWidget());
		CanvasPanel->AddChild(InNewWidget);
	}
}

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
		WidgetPosition += ScreenOffset;
		ParentOverlay->SetRenderTranslation(WidgetPosition);
	}

	// Avoid unnecessary invalidation in Slate
	if (bWasOnScreen != bIsOnScreen)
	{
		ParentOverlay->SetVisibility(bIsOnScreen ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}

	bWasOnScreen = bIsOnScreen;
}

void UUPWorldUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Assumes we are directly added to a CanvasPanel rather than direct to viewport.
	UCanvasPanelSlot* CanvasSlot = static_cast<UCanvasPanelSlot*>(Slot);

	// Might be nullptr during init
	if (CanvasSlot)
	{
		CanvasSlot->SetAlignment(FVector2d(0.5f,1.0f));
		CanvasSlot->SetAutoSize(true);
	}
}
