// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GBTService_CheckAttackRange.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

#if !UE_BUILD_SHIPPING
namespace DevelopmentOnly
{
	static bool GGameDrawDebugAttackRange = false;
	static FAutoConsoleVariableRef CVarGameDrawDebug_AttackRangeService(
		TEXT("game.DrawDebugAttackRange"),
		GGameDrawDebugAttackRange,
		TEXT("Enable debug rendering of the attack range services.\n"),
		ECVF_Cheat
		);
}
#endif

UGBTService_CheckAttackRange::UGBTService_CheckAttackRange()
{
	AttackRange = 500.0f;
	TargetActorKey.SelectedKeyName = "TargetActor";
}

void UGBTService_CheckAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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
		const bool bWithinRange = DistanceTo < AttackRange;

		BlackBoardComp->SetValueAsBool(AttackRangeKey.SelectedKeyName, bWithinRange);

#if !UE_BUILD_SHIPPING
		if (DevelopmentOnly::GGameDrawDebugAttackRange)
		{
			DrawDebugCircle(GetWorld(), Center, AttackRange, 32.0f, DebugColor, false, DeltaSeconds,
				0, 4, FVector(0,1,0), FVector::ForwardVector);
		}
#endif
	}
}
