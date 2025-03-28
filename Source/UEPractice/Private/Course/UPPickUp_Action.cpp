// Fill out your copyright notice in the Description page of Project Settings.

#include "Course/UPPickUp_Action.h"
#include "Course/UPAction.h"
#include "Course/UPActionComponent.h"

void AUPPickUp_Action::Interact_Implementation(APawn* InstigatorPawn)
{
	Super::Interact_Implementation(InstigatorPawn);

	// Make sure an action class was set up
	if (!ensureAlways(ActionToGrant))
 	{
 		return;
 	}
 
	UUPActionComponent* ActionComp = InstigatorPawn->FindComponentByClass<UUPActionComponent>();
	check(ActionComp);

 	// Check if Player already has action class
	if (ActionComp->GetAction(ActionToGrant))
 	{
		const FString DebugMsg = FString::Printf(TEXT("Action '%s' already known."), *GetNameSafe(ActionToGrant));
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, DebugMsg);
		return;
 	}

	// Give new Ability
	ActionComp->AddAction(InstigatorPawn, ActionToGrant);	
	HideAndCooldownPickUp();
}
