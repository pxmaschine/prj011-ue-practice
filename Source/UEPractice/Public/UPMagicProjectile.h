// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UPProjectileBase.h"
#include "UPMagicProjectile.generated.h"

class UAudioComponent;
class USoundCue;

UCLASS()
class UEPRACTICE_API AUPMagicProjectile : public AUPProjectileBase
{
	GENERATED_BODY()

public:
	AUPMagicProjectile();

protected:
	virtual void PostInitializeComponents() override;

	UFUNCTION()
	virtual void OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DamageAmount;
};
