// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UPPickUpActor.h"
#include "UPHealthPotionActor.generated.h"

/**
 * 
 */
UCLASS()
class UEPRACTICE_API AUPHealthPotionActor : public AUPPickUpActor
{
	GENERATED_BODY()

public:
	AUPHealthPotionActor();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComp;

public:
	virtual void Interact_Implementation(APawn* InstigatorPawn) override;
};
