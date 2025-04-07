// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/AI/UPBTTask_Heal.h"
#include "Course/UPActionComponent.h"
#include "Course/SharedGameplayTags.h"

#include "AIController.h"

EBTNodeResult::Type UUPBTTask_Heal::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APawn* MyPawn = OwnerComp.GetAIOwner()->GetPawn();

	UUPActionComponent* ActionComp = UUPActionComponent::GetActionComponent(MyPawn);
	check (ActionComp);
	
	const FUPAttribute* HealthMaxAttribute = ActionComp->GetAttribute(SharedGameplayTags::Attribute_HealthMax);

	// @todo: support CLAMPING in the attribute system, now it overheals the BOT beyond original max HP
	const FAttributeModification AttriMod = FAttributeModification(
		SharedGameplayTags::Attribute_Health,
		HealthMaxAttribute->GetValue(),
		ActionComp,
		MyPawn,
		EAttributeModifyType::AddDelta);

	ActionComp->ApplyAttributeChange(AttriMod);

	return EBTNodeResult::Succeeded;
}
