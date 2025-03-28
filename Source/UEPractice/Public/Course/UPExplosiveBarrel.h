// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UPExplosiveBarrel.generated.h"

class UStaticMeshComponent;
class URadialForceComponent;

UCLASS()
class UEPRACTICE_API AUPExplosiveBarrel : public AActor
{
	GENERATED_BODY()
	
public:
	AUPExplosiveBarrel();

protected:
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMesh{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	URadialForceComponent* RadialForce{ nullptr };
};
