// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Course/UPAttributeSet.h"
#include "GAttributeSet.generated.h"

USTRUCT(BlueprintType)
struct FGPlayerAttributeSet : public FUPAttributeSet
{
	GENERATED_BODY()

	FGPlayerAttributeSet()
	{
		AttackDamage = FUPAttribute(10.0f);
		Health = FUPAttribute(100.0f);
		HealthMax = FUPAttribute(Health.GetValue());
	}

	/* Base Damage value, all skills and damage use this multiplied by a damage coefficient
	 * (a percentage defaulting to 100%) to simplify balancing and scaling during play */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Attributes")
	FUPAttribute AttackDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Attributes")
	FUPAttribute Health;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Attributes")
	FUPAttribute HealthMax;
};

USTRUCT(BlueprintType)
struct FGEnemyAttributeSet : public FUPAttributeSet
{
	GENERATED_BODY()

	FGEnemyAttributeSet()
	{
		AttackDamage = FUPAttribute(10.0f);
		Health = FUPAttribute(100.0f);
		HealthMax = FUPAttribute(Health.GetValue());
	}

	/* Base Damage value, all skills and damage use this multiplied by a damage coefficient
	 * (a percentage defaulting to 100%) to simplify balancing and scaling during play */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Attributes")
	FUPAttribute AttackDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Attributes")
	FUPAttribute Health;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category= "Attributes")
	FUPAttribute HealthMax;
};
