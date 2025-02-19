// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Course/UPPickUpActor.h"
#include "UPCreditsActor.generated.h"


UCLASS()
class UEPRACTICE_API AUPCreditsActor : public AUPPickUpActor
{
	GENERATED_BODY()

public:
	AUPCreditsActor();

public:
	void Interact_Implementation(APawn* InstigatorPawn) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Credits")
	int32 CreditsAmount;
};
