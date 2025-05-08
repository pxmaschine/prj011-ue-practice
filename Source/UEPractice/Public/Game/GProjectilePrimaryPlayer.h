// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Course/UPProjectileBase.h"
#include "GProjectilePrimaryPlayer.generated.h"


class HomingTargetComponent;

UCLASS()
class UEPRACTICE_API AGProjectilePrimaryPlayer : public AUPProjectileBase
{
	GENERATED_BODY()
	
public:
	AGProjectilePrimaryPlayer();

public:
	UFUNCTION(BlueprintCallable)
	void SetTarget(AActor* TargetActor);

protected:
	virtual void PostInitializeComponents() override;

	UFUNCTION()
	virtual void OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

protected:
	UPROPERTY(EditAnywhere, Category = Effects)
	float ImpulseStrength;

	UPROPERTY(EditDefaultsOnly, Category = Damage, meta=(Units="Percent"))
	float DamageCoefficient;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	float HomingAcceleration;
};
