// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UPTargetDummy.generated.h"

class UUPAttributeComponent;

UCLASS()
class UEPRACTICE_API AUPTargetDummy : public APawn
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUPTargetDummy();

protected:
	virtual void PostInitializeComponents() override;

	//UPROPERTY(VisibleAnywhere)
	//UUPAttributeComponent* AttributeComp;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> SkelMeshComp;

	//UFUNCTION()
	//void OnHealthChanged(AActor* InstigatorActor, UUPAttributeComponent* OwningComp, float NewHealth, float Delta);
};
