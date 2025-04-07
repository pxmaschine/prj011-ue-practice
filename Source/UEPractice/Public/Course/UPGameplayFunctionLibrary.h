// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UPGameplayFunctionLibrary.generated.h"

class UUPActionComponent;

UCLASS()
class UEPRACTICE_API UUPGameplayFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category=Gameplay)
	static UUPActionComponent* GetActionComponentFromActor(const AActor* FromActor);

	UFUNCTION(BlueprintCallable, Category=Gameplay)
	static bool IsAlive(const AActor* InActor);

	UFUNCTION(BlueprintCallable, Category=Gameplay)
	static bool IsFullHealth(AActor* InActor);

	UFUNCTION(BlueprintCallable, Category=Gameplay)
	static bool ApplyDamage(AActor* DamageCauser, AActor* TargetActor, float DamageCoefficient);

	UFUNCTION(BlueprintCallable, Category=Gameplay)
	static bool ApplyDirectionalDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount, const FHitResult& HitResult);
/*
	static bool ApplyRadialDamage(AActor* DamageCauser, FVector Origin, float DamageRadius, float DamageCoefficient);
*/
	//void OnDamageOverlapComplete(const FTraceHandle& TraceHandle, FOverlapDatum& OverlapDatum, FDamageInfo DamageInfo);
		
	UFUNCTION(BlueprintPure, Category = "PSO Caching")
	static int32 GetRemainingBundledPSOs();
};
