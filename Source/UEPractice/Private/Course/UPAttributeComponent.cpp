// Fill out your copyright notice in the Description page of Project Settings.

#include "Course/UPAttributeComponent.h"

#include "Course/UPGameModeBase.h"
#include "Net/UnrealNetwork.h"

static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("game.DamageMultiplier"), 1.0f, TEXT("Global Damage Modifier for Attribute Component."), ECVF_Cheat);

// Sets default values for this component's properties
UUPAttributeComponent::UUPAttributeComponent()
{
	MaxHealth = 100.0f;
	Health = MaxHealth;

	MaxRage = 100.0f;
	Rage = 0.0f;

	SetIsReplicatedByDefault(true);
}

void UUPAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UUPAttributeComponent, Health);
	DOREPLIFETIME(UUPAttributeComponent, MaxHealth);
	DOREPLIFETIME(UUPAttributeComponent, Rage);
	DOREPLIFETIME(UUPAttributeComponent, MaxRage);

	//DOREPLIFETIME_CONDITION(UUPAttributeComponent, MaxHealth, COND_OwnerOnly)
	//DOREPLIFETIME_CONDITION(UUPAttributeComponent, MaxRage, COND_OwnerOnly)
}

UUPAttributeComponent* UUPAttributeComponent::GetAttributes(const AActor* FromActor)
{
	if (FromActor)
	{
		return FromActor->FindComponentByClass<UUPAttributeComponent>();
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
	return FMath::IsNearlyEqual(Health, MaxHealth);
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
	const float NewHealth = FMath::Clamp(Health + Delta, 0.0f, MaxHealth);

	const float ActualDelta = NewHealth - OldHealth;

	// Is Server?
	if (GetOwner()->HasAuthority())
	{
		Health = NewHealth;

		if (!FMath::IsNearlyZero(ActualDelta))
		{
			MulticastHealthChanged(InstigatorActor, Health, ActualDelta);
		}

		// Died
		if (ActualDelta < 0.0f && FMath::IsNearlyZero(Health))
		{
			if (AUPGameModeBase* GM = GetWorld()->GetAuthGameMode<AUPGameModeBase>())
			{
				GM->OnActorKilled(GetOwner(), InstigatorActor);
			}
		}
	}

	return !FMath::IsNearlyZero(ActualDelta);
}

bool UUPAttributeComponent::ApplyRageChange(AActor* InstigatorActor, float Delta)
{
	const float OldRage = Rage;

	Rage = FMath::Clamp(Rage + Delta, 0.0f, MaxRage);

	const float ActualDelta = -(OldRage - Rage);

	if (!FMath::IsNearlyZero(ActualDelta))
	{
		//OnRageChanged.Broadcast(InstigatorActor, this, Rage, ActualDelta);
		MulticastRageChanged(InstigatorActor, Rage, ActualDelta);
		return true;
	}

	return false;
}

void UUPAttributeComponent::MulticastHealthChanged_Implementation(AActor* InstigatorActor, float NewValue, float Delta)
{
	OnHealthChanged.Broadcast(InstigatorActor, this, NewValue, Delta);
}

void UUPAttributeComponent::MulticastRageChanged_Implementation(AActor* InstigatorActor, float NewValue, float Delta)
{
	OnRageChanged.Broadcast(InstigatorActor, this, NewValue, Delta);
}
