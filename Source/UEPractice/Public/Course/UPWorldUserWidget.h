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
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual void NativeConstruct() override;

public:
	UPROPERTY(BlueprintReadOnly, Transient, Category = "UI", meta = (ExposeOnSpawn=true))
	AActor* AttachedActor;

protected:
	UPROPERTY(EditAnywhere, Category = "UI")
	FVector WorldOffset;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> ParentOverlay;

	bool bWasOnScreen = true;
};
