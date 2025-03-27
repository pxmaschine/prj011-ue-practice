// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/AI/UPBTTask_Heal.h"
#include "Course/UPAttributeComponent.h"

#include "AIController.h"

EBTNodeResult::Type UUPBTTask_Heal::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (const AAIController* AIC = OwnerComp.GetAIOwner(); ensure(AIC))
	{
		if (const APawn* AIPawn = AIC->GetPawn(); ensure(AIPawn))
		{
			UUPAttributeComponent* AttributeComp = Cast<UUPAttributeComponent>(AIPawn->GetComponentByClass(UUPAttributeComponent::StaticClass()));

			AttributeComp->ApplyHealthChange(AIC->GetInstigator(), AttributeComp->GetMaxHealth());

			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}
