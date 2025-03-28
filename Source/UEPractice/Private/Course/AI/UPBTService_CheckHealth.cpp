// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/AI/UPBTService_CheckHealth.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Course/UPAttributeComponent.h"


UUPBTService_CheckHealth::UUPBTService_CheckHealth()
{
	LowHealthFraction = 0.3f;
}

void UUPBTService_CheckHealth::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	const APawn* AIPawn = OwnerComp.GetAIOwner()->GetPawn();

	UUPAttributeComponent* AttributeComp = UUPAttributeComponent::GetAttributes(AIPawn);
	if (ensure(AttributeComp))
	{
		const bool bLowHealth = (AttributeComp->GetCurrentHealth() / AttributeComp->GetMaxHealth()) < LowHealthFraction;

		UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();
		BlackBoardComp->SetValueAsBool(LowHealthKey.SelectedKeyName, bLowHealth);
	}
}
