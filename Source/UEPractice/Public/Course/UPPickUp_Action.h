// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Course/UPPickUpActor.h"
#include "UPPickUp_Action.generated.h"

class UUPAction;

UCLASS()
class UEPRACTICE_API AUPPickUp_Action : public AUPPickUpActor
{
	GENERATED_BODY()

protected:
 
 	UPROPERTY(EditAnywhere, Category = "Pickup")
 	TSubclassOf<UUPAction> ActionToGrant;
 
 public:
 	void Interact_Implementation(APawn* InstigatorPawn) override;
 };
