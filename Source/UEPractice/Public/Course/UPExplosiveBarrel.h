// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UPExplosiveBarrel.generated.h"

class UUPActionComponent;
struct FAttributeModification;
class UStaticMeshComponent;
class URadialForceComponent;
class UNiagaraComponent;

UCLASS()
class UEPRACTICE_API AUPExplosiveBarrel : public AActor
{
	GENERATED_BODY()
	
public:
	AUPExplosiveBarrel();

public:
	virtual void PostInitializeComponents() override;

protected:
	void OnHealthAttributeChanged(float NewValue, const FAttributeModification& AttributeModification);

protected:
	void Explode();

protected:
	UPROPERTY(VisibleAnywhere, Category = Components)
	TObjectPtr<UUPActionComponent> ActionComp;

	UPROPERTY(VisibleAnywhere, Category = Components)
	UStaticMeshComponent* StaticMesh{ nullptr };

	UPROPERTY(VisibleAnywhere, Category = Components)
	URadialForceComponent* RadialForce{ nullptr };

	UPROPERTY(VisibleAnywhere, Category = Components)
	TObjectPtr<UNiagaraComponent> ExplosionComp;

	UPROPERTY(VisibleAnywhere, Category = Components)
	TObjectPtr<UNiagaraComponent> FlamesFXComp;

	int32 HitCounter = 0;

	FTimerHandle DelayedExplosionHandle;

	float ExplosionDelayTime;

	bool bExploded = false;
};
