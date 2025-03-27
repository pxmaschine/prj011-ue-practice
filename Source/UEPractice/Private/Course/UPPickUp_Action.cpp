// Fill out your copyright notice in the Description page of Project Settings.

#include "Course/UPPickUp_Action.h"
#include "Course/UPAction.h"
#include "Course/UPActionComponent.h"

void AUPPickUp_Action::Interact_Implementation(APawn* InstigatorPawn)
{
	Super::Interact_Implementation(InstigatorPawn);

	// Make sure we have instigator & that action class was set up
 	if (!ensure(InstigatorPawn && ActionToGrant))
 	{
 		return;
 	}
 
 	UUPActionComponent* ActionComp = Cast<UUPActionComponent>(InstigatorPawn->GetComponentByClass(UUPActionComponent::StaticClass()));
 	// Check if Player already has action class
 	if (ActionComp)
 	{
 		if (ActionComp->GetAction(ActionToGrant))
 		{
 			FString DebugMsg = FString::Printf(TEXT("Action '%s' already known."), *GetNameSafe(ActionToGrant));
 			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, DebugMsg);
 			return;
 		}
 
 		// Give new Ability
 		ActionComp->AddAction(InstigatorPawn, ActionToGrant);	
 		HideAndCooldownPickUp();
 	}
}
