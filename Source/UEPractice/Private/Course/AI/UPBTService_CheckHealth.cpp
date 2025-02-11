// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/AI/UPBTService_CheckHealth.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Course/AI/UPAICharacter.h"


void UUPBTService_CheckHealth::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent(); ensure(BlackboardComponent))
	{
		if (const AAIController* AIController = OwnerComp.GetAIOwner(); ensure(AIController))
		{
			if (const AUPAICharacter* AICharacter = Cast<AUPAICharacter>(AIController->GetCharacter()); ensure(AICharacter))
			{
				BlackboardComponent->SetValueAsBool(LowHealthKey.SelectedKeyName, AICharacter->HasLowHealth());
			}
		}
	}
}
