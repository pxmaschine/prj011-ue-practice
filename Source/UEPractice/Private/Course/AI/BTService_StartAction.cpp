// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/AI/BTService_StartAction.h"
#include "Course/UPActionComponent.h"

#include "AIController.h"

void UBTService_StartAction::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// Assumed AIController and Pawn never nullptr (BT should be disabled if so)
	APawn* MyPawn = OwnerComp.GetAIOwner()->GetPawn();
	check(MyPawn);
	
	UUPActionComponent* ActionComp = MyPawn->FindComponentByClass<UUPActionComponent>();
	check(ActionComp); // If nullptr we haven't properly implemented the enemy with an action component
	
	ActionComp->StartActionByName(MyPawn, ActionName);
}
