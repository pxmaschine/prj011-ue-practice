// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPHealthPotionActor.h"
#include "Course/UPAttributeComponent.h"
#include "Course/UPPlayerState.h"


#define LOCTEXT_NAMESPACE "InteractableActors"

AUPHealthPotionActor::AUPHealthPotionActor()
{
	CreditCost = 50;
}

void AUPHealthPotionActor::Interact_Implementation(APawn* InstigatorPawn)
{
	Super::Interact_Implementation(InstigatorPawn);

	if (!ensure(InstigatorPawn))
	{
		return;
	}

	UUPAttributeComponent* AttributeComp = Cast<UUPAttributeComponent>(InstigatorPawn->GetComponentByClass(UUPAttributeComponent::StaticClass()));
	// Check if not already at max health
	if (ensure(AttributeComp) && !AttributeComp->IsFullHealth())
	{
		
		if (AUPPlayerState* PS = InstigatorPawn->GetPlayerState<AUPPlayerState>())
		{
			if (PS->RemoveCredits(CreditCost) && AttributeComp->ApplyHealthChange(this, AttributeComp->GetMaxHealth()))
			{
				// Only activate if healed successfully
				HideAndCooldownPickUp();
			}
		}
	}
}

FText AUPHealthPotionActor::GetInteractText_Implementation(APawn* InstigatorPawn)
{
	const UUPAttributeComponent* AttributeComp = Cast<UUPAttributeComponent>(InstigatorPawn->GetComponentByClass(UUPAttributeComponent::StaticClass()));

	if (ensure(AttributeComp) && AttributeComp->IsFullHealth())
	{
		return LOCTEXT("HealthPotion_FullHealthWarning", "Already at full health.");
	}

	return FText::Format(LOCTEXT("HealthPotion_InteractMessage", "Cost {0} Credits. Restores health to maximum."), CreditCost);
}

#undef LOCTEXT_NAMESPACE
