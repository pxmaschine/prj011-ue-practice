// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPHealthPotionActor.h"
#include "Course/SharedGameplayTags.h"
#include "Course/UPActionComponent.h"
#include "Course/UPGameplayFunctionLibrary.h"
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

	UUPActionComponent* ActionComp = UUPActionComponent::GetActionComponent(InstigatorPawn);
	// Check if not already at max health
	if (ensure(ActionComp) && !UUPGameplayFunctionLibrary::IsFullHealth(InstigatorPawn))
	{
		
		if (AUPPlayerState* PS = InstigatorPawn->GetPlayerState<AUPPlayerState>())
		{
			if (PS->TryRemoveCredits(CreditCost))
			{
				if (ActionComp->ApplyAttributeChange(
					SharedGameplayTags::Attribute_Health,
					ActionComp->GetAttribute(SharedGameplayTags::Attribute_HealthMax)->GetValue(),
					this,
					EAttributeModifyType::AddModifier))
				{
					// Only activate if healed successfully
					HideAndCooldownPickUp();
				}
			}
		}
	}
}

FText AUPHealthPotionActor::GetInteractText_Implementation(APawn* InstigatorPawn)
{
	if (UUPGameplayFunctionLibrary::IsFullHealth(InstigatorPawn))
	{
		return LOCTEXT("HealthPotion_FullHealthWarning", "Already at full health.");
	}

	return FText::Format(LOCTEXT("HealthPotion_InteractMessage", "Cost {0} Credits. Restores health to maximum."), CreditCost);
}

#undef LOCTEXT_NAMESPACE
