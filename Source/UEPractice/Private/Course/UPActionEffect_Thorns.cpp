// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPActionEffect_Thorns.h"

#include "Course/SharedGameplayTags.h"
#include "Course/UPActionComponent.h"
#include "Course/UPGameplayFunctionLibrary.h"


UUPActionEffect_Thorns::UUPActionEffect_Thorns()
{
	Duration = 0.0f;
	Period = 0.0f;

	ReflectFraction = 0.2f;
}

void UUPActionEffect_Thorns::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	// Start listening
	AttriChangedHandle = ActionComp->GetAttribute(SharedGameplayTags::Attribute_Health)->OnAttributeChanged.AddUObject(this, &ThisClass::OnHealthChanged);
}

void UUPActionEffect_Thorns::StopAction_Implementation(AActor* Instigator)
{
	Super::StopAction_Implementation(Instigator);

	// Stop listening
	ActionComp->GetAttribute(SharedGameplayTags::Attribute_Health)->OnAttributeChanged.Remove(AttriChangedHandle);
}

void UUPActionEffect_Thorns::OnHealthChanged(float NewValue, const FAttributeModification& AttributeModification)
{
	AActor* OwningActor = GetOwningComponent()->GetOwner();

	// Damage Only
	if (AttributeModification.Magnitude < 0.0f && OwningActor != AttributeModification.Instigator)
	{
		/*
		// Round to nearest to avoid 'ugly' damage numbers and tiny reflections
		int32 ReflectedAmount = FMath::RoundToInt(AttributeModification.Magnitude * ReflectFraction);
		if (ReflectedAmount == 0)
		{
			return;
		}

		// Flip to positive, so we don't end up healing ourselves when passed into damage
		ReflectedAmount = FMath::Abs(ReflectedAmount);*/

		// @todo: maybe thorns can still base DMG on base dmg from hit rather than using player baseDmg attribute as with all normal damage
		float DmgCoefficient = 5.0f;

		// Return damage to sender...
		UUPGameplayFunctionLibrary::ApplyDamage(OwningActor, AttributeModification.Instigator.Get(), DmgCoefficient);
	}
}
