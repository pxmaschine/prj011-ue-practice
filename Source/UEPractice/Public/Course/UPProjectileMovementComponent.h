// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "UPProjectileMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class UEPRACTICE_API UUPProjectileMovementComponent : public UProjectileMovementComponent
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	void Reset();
};
