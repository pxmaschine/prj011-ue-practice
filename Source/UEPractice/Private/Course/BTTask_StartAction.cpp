// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/BTTask_StartAction.h"
#include "Course/UPActionComponent.h"

#include "AIController.h"

EBTNodeResult::Type UBTTask_StartAction::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* MyController = OwnerComp.GetAIOwner();
	if (ensure(MyController))
	{
		APawn* MyPawn = MyController->GetPawn();
		if (MyPawn == nullptr)
		{
			return EBTNodeResult::Failed;
		}
		
		UUPActionComponent* ActionComp = Cast<UUPActionComponent>(MyPawn->GetComponentByClass(UUPActionComponent::StaticClass()));
		if (ensure(ActionComp))
		{
			if (ActionComp->StartActionByName(MyPawn, ActionName))
			{
				return EBTNodeResult::Succeeded;
			}
		}
	}

	return EBTNodeResult::Failed;
}
