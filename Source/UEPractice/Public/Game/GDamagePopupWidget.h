// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Course/UPWorldUserWidget.h"
#include "GDamagePopupWidget.generated.h"


UCLASS()
class UEPRACTICE_API UGDamagePopupWidget : public UUPWorldUserWidget
{
	GENERATED_BODY()

public:
	void SetDamageText(const FString& Damage) { DamageText = Damage; }

protected:
	UPROPERTY(Transient, BlueprintReadOnly)
	FString DamageText;
};
