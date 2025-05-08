// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UPWorldUserWidget.generated.h"

class UOverlay;

UCLASS()
class UEPRACTICE_API UUPWorldUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/* Find the root canvas panel (expected in a 'main hud' style widget) and add new widget as child. */
	UFUNCTION(BlueprintCallable, Category= UI)
	static void AddToRootCanvasPanel(UUserWidget* InNewWidget);

protected:
	virtual void NativeConstruct() override;
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	UPROPERTY(BlueprintReadOnly, Transient, Category = "UI", meta = (ExposeOnSpawn=true))
	AActor* AttachedActor;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FVector WorldOffset;

	UPROPERTY(Transient, BlueprintReadWrite)
	FVector2D ScreenOffset{ 0.0, 0.0 };

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> ParentOverlay;

	bool bWasOnScreen = true;
};
