// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/AI/BTTask_StartAction.h"
#include "Course/UPActionComponent.h"

#include "AIController.h"

EBTNodeResult::Type UBTTask_StartAction::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Assumed AIController and Pawn never nullptr (BT should be disabled if so)
	APawn* MyPawn = OwnerComp.GetAIOwner()->GetPawn();
	check(MyPawn);
	
	UUPActionComponent* ActionComp = MyPawn->FindComponentByClass<UUPActionComponent>();
	check(ActionComp); // If nullptr we haven't properly implemented the enemy with an action component
	
	if (ActionComp->StartActionByName(MyPawn, ActionName))
	{
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
