// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Course/UPActionEffect.h"
#include "UPActionEffect_Thorns.generated.h"

struct FAttributeModification;

UCLASS()
class UEPRACTICE_API UUPActionEffect_Thorns : public UUPActionEffect
{
	GENERATED_BODY()
	
public:
	UUPActionEffect_Thorns();

public:
	virtual void StartAction_Implementation(AActor* Instigator) override;
	virtual void StopAction_Implementation(AActor* Instigator) override;

protected:
	UFUNCTION()
	void OnHealthChanged(float NewValue, const FAttributeModification& AttributeModification);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Thorns")
	float ReflectFraction;

	FDelegateHandle AttriChangedHandle;
};
