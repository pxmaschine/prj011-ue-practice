// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/AI/UPBTTask_Heal.h"

#include "AIController.h"
#include "Course/AI/UPAICharacter.h"

EBTNodeResult::Type UUPBTTask_Heal::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (const APawn* AIPawn = OwnerComp.GetAIOwner()->GetPawn(); ensure(AIPawn))
	{
		UUPAttributeComponent* AttributeComp = Cast<UUPAttributeComponent>(AIPawn->GetComponentByClass(UUPAttributeComponent::StaticClass()));

		AttributeComp->ApplyHealthChange(AttributeComp->GetMaxHealth());

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
