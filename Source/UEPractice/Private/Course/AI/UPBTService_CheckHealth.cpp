// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/AI/UPBTService_CheckHealth.h"
#include "Course/UPActionComponent.h"
#include "Course/SharedGameplayTags.h"
#include "Course/UPGameplayFunctionLibrary.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"


UUPBTService_CheckHealth::UUPBTService_CheckHealth()
{
	LowHealthFraction = 0.3f;
}

void UUPBTService_CheckHealth::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	const APawn* AIPawn = OwnerComp.GetAIOwner()->GetPawn();
	check(AIPawn);

	UUPActionComponent* ActionComp = UUPGameplayFunctionLibrary::GetActionComponentFromActor(AIPawn);
	check(ActionComp);

	const float Health = ActionComp->GetAttribute(SharedGameplayTags::Attribute_Health)->GetValue();
	const float HealthMax = ActionComp->GetAttribute(SharedGameplayTags::Attribute_HealthMax)->GetValue();

	const bool bLowHealth = (Health / HealthMax) < LowHealthFraction;

	UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();
	BlackBoardComp->SetValueAsBool(LowHealthKey.SelectedKeyName, bLowHealth);
}
