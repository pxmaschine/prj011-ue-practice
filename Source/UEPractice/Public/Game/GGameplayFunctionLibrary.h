// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GGameplayFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class UEPRACTICE_API UGGameplayFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category=Gameplay)
	static AActor* GetClosestEnemy(const AActor* FromActor);
};
