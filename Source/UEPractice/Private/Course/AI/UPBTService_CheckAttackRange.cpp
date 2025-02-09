// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/AI/UPBTService_CheckAttackRange.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"


void UUPBTService_CheckAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// Check distance between ai pawn and target actor

	if (UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent(); ensure(BlackboardComponent))
	{
		if (const AActor* TargetActor = Cast<AActor>(BlackboardComponent->GetValueAsObject("TargetActor")))
		{
			if (const AAIController* AIController = OwnerComp.GetAIOwner(); ensure(AIController))
			{
				if (const APawn* AIPawn = AIController->GetPawn(); ensure(AIPawn))
				{
					const float DistanceTo = FVector::Distance(TargetActor->GetActorLocation(), AIPawn->GetActorLocation());

					const bool bWithinRange = DistanceTo < 2000.0f;

					bool bHasLOS = false;
					if (DistanceTo)
					{
						bHasLOS = AIController->LineOfSightTo(TargetActor);
					}

					BlackboardComponent->SetValueAsBool(AttackRangeKey.SelectedKeyName, bWithinRange && bHasLOS);
				}
			}
		}
	}
}
