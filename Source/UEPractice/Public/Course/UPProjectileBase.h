// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UPActorPoolingInterface.h"
#include "GameFramework/Actor.h"
#include "UPProjectileBase.generated.h"

class USphereComponent;
class UUPProjectileMovementComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class UCameraShakeBase;
class USoundCue;

UCLASS(Abstract)
class UEPRACTICE_API AUPProjectileBase : public AActor, public IUPActorPoolingInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUPProjectileBase();

protected:
	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	virtual void OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// BlueprintNativeEvent = C++ base implementation; can be expanded in blueprints
	// BlueprintCallable = Allow child classes to trigger explosions
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Explode();

	virtual void LifeSpanExpired() override;

	virtual void PoolBeginPlay_Implementation() override;

	virtual void PoolEndPlay_Implementation() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Effects|Shake")
	TSubclassOf<UCameraShakeBase> ImpactShake;

	UPROPERTY(EditDefaultsOnly, Category = "Effects|Shake")
	float ImpactShakeInnerRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Effects|Shake")
	float ImpactShakeOuterRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UNiagaraSystem* ImpactVFX;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	USoundCue* ImpactSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UUPProjectileMovementComponent* MovementComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UNiagaraComponent> NiagaraLoopComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAudioComponent* AudioComp;
};
