// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "UPBTService_CheckAttackRange.generated.h"

/**
 * 
 */
UCLASS()
class UEPRACTICE_API UUPBTService_CheckAttackRange : public UBTService
{
	GENERATED_BODY()

public:
	UUPBTService_CheckAttackRange();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
//#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = Debug)
	FColor DebugColor = FColor::Green;
//#endif

	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector AttackRangeKey;

	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector TargetActorKey;

	/* Max desired attack range of AI pawn */
	UPROPERTY(EditAnywhere, Category = "AI")
	float MaxAttackRange;
};
