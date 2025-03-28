// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/AI/UPBTTask_Heal.h"
#include "Course/UPAttributeComponent.h"

#include "AIController.h"

EBTNodeResult::Type UUPBTTask_Heal::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APawn* MyPawn = OwnerComp.GetAIOwner()->GetPawn();

	UUPAttributeComponent* AttributeComp = UUPAttributeComponent::GetAttributes(MyPawn);
	if (ensure(AttributeComp))
	{
		AttributeComp->ApplyHealthChange(MyPawn, AttributeComp->GetMaxHealth());
	}

	return EBTNodeResult::Succeeded;
}
