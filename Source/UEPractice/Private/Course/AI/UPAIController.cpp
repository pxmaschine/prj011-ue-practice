// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/AI/UPAIController.h"
#include "Course/UPCharacter.h"
#include "Course/AI/UPAICharacter.h"
#include "Course/AI/UPBTService_CheckAttackRange.h"
#include "UEPractice/UEPractice.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"


AUPAIController::AUPAIController()
{
	PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
}

void AUPAIController::BeginPlay()
{
	Super::BeginPlay();

	// Set default for all AI
	SetGenericTeamId(FGenericTeamId(TEAM_ID_BOTS));

	if (ensureMsgf(BehaviorTree, TEXT("Behavior Tree is nullptr! Please assign BehaviorTree in your AI Controller.")))
	{
		RunBehaviorTree(BehaviorTree);
	}

	// Setup a listener to check black board for target getting set so we can notify (clients) via a UI popup that they were spotted	
	UBlackboardComponent* BBComp = GetBlackboardComponent();
	BBComp->RegisterObserver(BBComp->GetKeyID(TEXT("TargetActor")), this,
		FOnBlackboardChangeNotification::CreateUObject(this, &ThisClass::OnTargetActorChanged));
}

EBlackboardNotificationResult AUPAIController::OnTargetActorChanged(const UBlackboardComponent& Comp,
	FBlackboard::FKey KeyID)
{
	// Filter to only players as something we want to notify
	if (AUPCharacter* TargetPlayer = Cast<AUPCharacter>(Comp.GetValueAsObject("TargetActor")))
	{
		// This will be running on the host/server as AI controllers only exist there, therefor we run a Client RPC on the character
		// so the local player will see the notification
		TargetPlayer->ClientOnSeenBy(CastChecked<AUPAICharacter>(GetPawn()));

		// In our current behavior, we can remove the listener after first successful trigger
		//return EBlackboardNotificationResult::RemoveObserver;
	}

	return EBlackboardNotificationResult::ContinueObserving;
}
