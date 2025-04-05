// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/AI/UPBTService_CheckAttackRange.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

#if !UE_BUILD_SHIPPING
namespace DevelopmentOnly
{
	static bool GDrawDebugAttackRange = false;
	static FAutoConsoleVariableRef CVarDrawDebug_AttackRangeService(
		TEXT("up.DrawDebugAttackRange"),
		GDrawDebugAttackRange,
		TEXT("Enable debug rendering of the attack range services.\n"),
		ECVF_Cheat
		);
}
#endif

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

		const FVector Center = MyController->GetPawn()->GetActorLocation();

		const float DistanceTo = FVector::Distance(TargetActor->GetActorLocation(), Center);
		const bool bWithinRange = DistanceTo < MaxAttackRange;

		bool bHasLOS = false;
		if (bWithinRange)
		{
			bHasLOS = MyController->LineOfSightTo(TargetActor);
		}

		BlackBoardComp->SetValueAsBool(AttackRangeKey.SelectedKeyName, (bWithinRange && bHasLOS));

#if !UE_BUILD_SHIPPING
		if (DevelopmentOnly::GDrawDebugAttackRange)
		{
			DrawDebugCircle(GetWorld(), Center, MaxAttackRange, 32.0f, DebugColor, false, DeltaSeconds,
				0, 4, FVector(0,1,0), FVector::ForwardVector);
		}
#endif
	}
}
