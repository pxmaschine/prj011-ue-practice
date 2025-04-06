// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Course/UPAction.h"
#include "UPAction_ProjectileAttack.generated.h"

class UAnimMontage;
class UNiagaraSystem;
class AUPCharacter;

UCLASS()
class UEPRACTICE_API UUPAction_ProjectileAttack : public UUPAction
{
	GENERATED_BODY()

public:
	UUPAction_ProjectileAttack();

public:
	virtual void StartAction_Implementation(AActor* Instigator) override;
	
protected:
	void AttackDelay_Elapsed(class AUPCharacter* InstigatorCharacter);

protected:
	/* Sphere radius of the sweep to find desired target under crosshair. Adjusts final projectile direction */
	UPROPERTY(EditAnywhere, Category="Targeting")
	float SweepRadius;

	/* Fallback distance when sweep finds no collision under crosshair. Adjusts final projectile direction */
	UPROPERTY(EditAnywhere, Category="Targeting")
	float SweepDistanceFallback;

	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(VisibleAnywhere, Category = "Effects")
	FName HandSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float AttackAnimDelay;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* AttackAnim;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UNiagaraSystem* CastVFX;

	/* Sound Effect to play (Can be Wave or Cue) */
	UPROPERTY(EditAnywhere, Category = "Attack")
	TObjectPtr<USoundBase> CastingSound;

};
