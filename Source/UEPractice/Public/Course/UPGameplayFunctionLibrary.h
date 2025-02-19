// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UPGameplayFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class UEPRACTICE_API UUPGameplayFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	static bool ApplyDamage(AActor* DamageInstigator, AActor* TargetActor, float DamageAmount);

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	static bool ApplyDirectionalDamage(AActor* DamageInstigator, AActor* TargetActor, float DamageAmount, const FHitResult& HitResult);
};
