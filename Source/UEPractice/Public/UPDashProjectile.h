// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UPProjectileBase.h"
#include "UPDashProjectile.generated.h"

/**
 * 
 */
UCLASS()
class UEPRACTICE_API AUPDashProjectile : public AUPProjectileBase
{
	GENERATED_BODY()

public:
	AUPDashProjectile();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Teleport")
	float TeleportDelay;

	UPROPERTY(EditDefaultsOnly, Category = "Teleport")
	float DetonateDelay;

	FTimerHandle TimerHandle_DelayedDetonate;

	virtual void Explode_Implementation() override;

	void TeleportInstigator();

	virtual void BeginPlay() override;
};
