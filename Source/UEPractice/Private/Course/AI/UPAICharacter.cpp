// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/AI/UPAICharacter.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/PawnSensingComponent.h"


AUPAICharacter::AUPAICharacter()
{
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");
}

void AUPAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PawnSensingComp->OnSeePawn.AddDynamic(this, &AUPAICharacter::OnPawnSeen);
}

void AUPAICharacter::OnPawnSeen(APawn* Pawn)
{
	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		UBlackboardComponent* BBComp = AIC->GetBlackboardComponent();

		BBComp->SetValueAsObject("TargetActor", Pawn);

		DrawDebugString(GetWorld(), Pawn->GetActorLocation(), "PLAYER SPOTTED", nullptr, FColor::White, 4.0f, true);
	}
}
