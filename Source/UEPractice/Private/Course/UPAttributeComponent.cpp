// Fill out your copyright notice in the Description page of Project Settings.

#include "Course/UPAttributeComponent.h"

// Sets default values for this component's properties
UUPAttributeComponent::UUPAttributeComponent()
{
	MaxHealth = 100.0f;
	Health = MaxHealth;
}

UUPAttributeComponent* UUPAttributeComponent::GetAttributes(const AActor* FromActor)
{
	if (FromActor)
	{
		return FromActor->GetComponentByClass<UUPAttributeComponent>();
	}

	return nullptr;
}

bool UUPAttributeComponent::IsActorAlive(const AActor* FromActor)
{
	if (const UUPAttributeComponent* Attributes = GetAttributes(FromActor))
	{
		return Attributes->IsAlive();
	}

	return false;
}

bool UUPAttributeComponent::IsAlive() const
{
	return Health > 0.0f;
}

bool UUPAttributeComponent::IsFullHealth() const
{
	return !(Health < MaxHealth);
}

bool UUPAttributeComponent::ApplyHealthChange(AActor* InstigatorActor, float Delta)
{
	const float OldHealth = Health;

	Health = FMath::Clamp(Health + Delta, 0.0f, MaxHealth);

	const float ActualDelta = -(OldHealth - Health);
	OnHealthChanged.Broadcast(InstigatorActor, this, Health, ActualDelta);

	return ActualDelta != 0;
}
