// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "UPBTTask_RangedAttack.generated.h"

/**
 * Original Ranged Attack class for AI. Replaced with Action System variant
 */
UCLASS(Deprecated) // 
class UEPRACTICE_API UDEPRECATED_UUPBTTask_RangedAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UDEPRECATED_UUPBTTask_RangedAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = "AI")
	float MaxBulletSpread;

	UPROPERTY(EditAnywhere, Category = "AI")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector TargetActorKey;

	UPROPERTY(EditAnywhere, Category= "AI")
	FName MuzzleSocket;
};
