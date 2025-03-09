// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPActionEffect_Thorns.h"
#include "Course/UPActionComponent.h"
#include "Course/UPAttributeComponent.h"
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
	if (UUPAttributeComponent* Attributes = UUPAttributeComponent::GetAttributes(GetOwningComponent()->GetOwner()))
 	{
 		Attributes->OnHealthChanged.AddDynamic(this, &UUPActionEffect_Thorns::OnHealthChanged);
 	}
}

void UUPActionEffect_Thorns::StopAction_Implementation(AActor* Instigator)
{
	Super::StopAction_Implementation(Instigator);

	// Stop listening
	if (UUPAttributeComponent* Attributes = UUPAttributeComponent::GetAttributes(GetOwningComponent()->GetOwner()))
 	{
 		Attributes->OnHealthChanged.RemoveDynamic(this, &UUPActionEffect_Thorns::OnHealthChanged);
 	}
}

void UUPActionEffect_Thorns::OnHealthChanged(AActor* InstigatorActor, UUPAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	AActor* OwningActor = GetOwningComponent()->GetOwner();
 
 	// Damage Only
 	if (Delta < 0.0f && OwningActor != InstigatorActor)
 	{
 		// Round to nearest to avoid 'ugly' damage numbers and tiny reflections
 		int32 ReflectedAmount = FMath::RoundToInt(Delta * ReflectFraction);
 		if (ReflectedAmount == 0)
 		{
 			return;
 		}
 
 		// Flip to positive, so we don't end up healing ourselves when passed into damage
 		ReflectedAmount = FMath::Abs(ReflectedAmount);
 
 		// Return damage sender...
 		UUPGameplayFunctionLibrary::ApplyDamage(OwningActor, InstigatorActor, ReflectedAmount);
 	}
}
