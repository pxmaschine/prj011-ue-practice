// Fill out your copyright notice in the Description page of Project Settings.

#include "UPAttributeComponent.h"

// Sets default values for this component's properties
UUPAttributeComponent::UUPAttributeComponent()
{
	Health = 100.0;
}

bool UUPAttributeComponent::ApplyHealthChange(float Delta)
{
	Health += Delta;

	OnHealthChanged.Broadcast(nullptr, this, Health, Delta);

	return true;
}
