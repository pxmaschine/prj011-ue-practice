// Fill out your copyright notice in the Description page of Project Settings.

#include "UPAttributeComponent.h"

// Sets default values for this component's properties
UUPAttributeComponent::UUPAttributeComponent()
{
	MaxHealth = 100.0f;
	Health = MaxHealth;
}

bool UUPAttributeComponent::IsAlive() const
{
	return Health > 0.0f;
}

bool UUPAttributeComponent::IsFullHealth() const
{
	return !(Health < MaxHealth);
}

bool UUPAttributeComponent::ApplyHealthChange(float Delta)
{
	Health += Delta;
	Health = FMath::Clamp(Health, 0.0f, MaxHealth);

	OnHealthChanged.Broadcast(nullptr, this, Health, Delta);

	return true;
}
