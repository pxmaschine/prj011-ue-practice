// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/AI/UPAIController.h"
#include "UEPractice/UEPractice.h"
#include "Perception/AIPerceptionComponent.h"


AUPAIController::AUPAIController()
{
	PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
}

void AUPAIController::BeginPlay()
{
	Super::BeginPlay();

	// Set default for for all AI
	SetGenericTeamId(FGenericTeamId(TEAM_ID_BOTS));

	if (ensureMsgf(BehaviorTree, TEXT("Behavior Tree is nullptr! Please assign BehaviorTree in your AI Controller.")))
	{
		RunBehaviorTree(BehaviorTree);
	}
}
