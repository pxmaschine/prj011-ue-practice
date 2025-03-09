// Fill out your copyright notice in the Description page of Project Settings.

#include "Course/UPAttributeComponent.h"

#include "Course/UPGameModeBase.h"

static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("up_DamageMultiplier"), 1.0f, TEXT("Global damage modifier for attribute component."), ECVF_Cheat);


// Sets default values for this component's properties
UUPAttributeComponent::UUPAttributeComponent()
{
	MaxHealth = 100.0f;
	Health = MaxHealth;

	MaxRage = 100.0f;
	Rage = 0.0f;
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

bool UUPAttributeComponent::Kill(AActor* InstigatorActor)
{
	return ApplyHealthChange(InstigatorActor, -GetMaxHealth());
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
	if (!GetOwner()->CanBeDamaged() && Delta < 0.0f)
	{
		return false;
	}

	if (Delta < 0.0f)
	{
		const float DamageMultiplier = CVarDamageMultiplier.GetValueOnGameThread();

		Delta *= DamageMultiplier;
	}

	const float OldHealth = Health;

	Health = FMath::Clamp(Health + Delta, 0.0f, MaxHealth);

	const float ActualDelta = -(OldHealth - Health);
	OnHealthChanged.Broadcast(InstigatorActor, this, Health, ActualDelta);

	// Died
	if (ActualDelta < 0.0f && Health == 0.0f)
	{
		if (AUPGameModeBase* GM = GetWorld()->GetAuthGameMode<AUPGameModeBase>())
		{
			GM->OnActorKilled(GetOwner(), InstigatorActor);
		}
	}

	return ActualDelta != 0;
}

bool UUPAttributeComponent::ApplyRageChange(AActor* InstigatorActor, float Delta)
{
	const float OldRage = Rage;

	Rage = FMath::Clamp(Rage + Delta, 0.0f, MaxRage);

	const float ActualDelta = -(OldRage - Rage);

	if (!FMath::IsNearlyZero(ActualDelta))
	{
		OnRageChanged.Broadcast(InstigatorActor, this, Rage, ActualDelta);
		return true;
	}

	return false;
}
