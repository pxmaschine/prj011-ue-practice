// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/AI/UPBTService_CheckAttackRange.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"


UUPBTService_CheckAttackRange::UUPBTService_CheckAttackRange()
{
	MaxAttackRange = 2000.f;

	TargetActorKey.SelectedKeyName = "TargetActor";
}

void UUPBTService_CheckAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// Check distance between ai pawn and target actor
	UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();
	check(BlackBoardComp);

	if (const AActor* TargetActor = Cast<AActor>(BlackBoardComp->GetValueAsObject(TargetActorKey.SelectedKeyName)))
	{
		const AAIController* MyController = OwnerComp.GetAIOwner();
		check(MyController);
		
		const float DistanceTo = FVector::Distance(TargetActor->GetActorLocation(), MyController->GetPawn()->GetActorLocation());
		const bool bWithinRange = DistanceTo < MaxAttackRange;

		bool bHasLOS = false;
		if (bWithinRange)
		{
			bHasLOS = MyController->LineOfSightTo(TargetActor);
		}

		BlackBoardComp->SetValueAsBool(AttackRangeKey.SelectedKeyName, (bWithinRange && bHasLOS));
	}
}
