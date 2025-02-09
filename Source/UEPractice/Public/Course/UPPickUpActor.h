// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UPGameplayInterface.h"
#include "GameFramework/Actor.h"
#include "UPPickUpActor.generated.h"

class USphereComponent;

UCLASS(Abstract)
class UEPRACTICE_API AUPPickUpActor : public AActor, public IUPGameplayInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUPPickUpActor();

protected:
	UPROPERTY(EditAnywhere, Category = "PickUp")
	float RespawnTime;

	UFUNCTION()
	void ShowPickUp();

	void HideAndCooldownPickUp();

	void SetPickUpState(bool bNewIsActive);

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* SphereComp;
};
