// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "UPAIController.generated.h"

class UBehaviorTree;
class UAIPerceptionComponent;

UCLASS()
class UEPRACTICE_API AUPAIController : public AAIController
{
	GENERATED_BODY()

public:
	AUPAIController();

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBehaviorTree* BehaviorTree;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAIPerceptionComponent> PerceptionComp;
};
