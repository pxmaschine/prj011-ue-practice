// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Course/UPAction.h"
#include "UPAction_MinionRangedAttack.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class UEPRACTICE_API UUPAction_MinionRangedAttack : public UUPAction
{
	GENERATED_BODY()

	/* Max Random Bullet Spread (in Degrees) in positive and negative angle (shared between Yaw and Pitch) */
	UPROPERTY(EditAnywhere, Category = "AI")
	float MaxBulletSpread = 2.0f;

	UPROPERTY(EditAnywhere, Category = "AI")
	TSubclassOf<AActor> ProjectileClass;

	virtual void StartAction_Implementation(AActor* Instigator) override;
};
