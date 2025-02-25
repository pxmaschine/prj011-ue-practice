// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPAction.h"

#include "Course/UPActionComponent.h"

void UUPAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Running: %s"), *GetNameSafe(this));

	UUPActionComponent* Comp = GetOwningComponent();
	Comp->ActiveGameplayTags.AppendTags(GrantedTags);

	bIsRunning = true;
}

void UUPAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Stopped: %s"), *GetNameSafe(this));

	ensureAlways(bIsRunning);

	UUPActionComponent* Comp = GetOwningComponent();
	Comp->ActiveGameplayTags.RemoveTags(GrantedTags);

	bIsRunning = false;
}

UWorld* UUPAction::GetWorld() const
{
	// Outer is set when creating action via NewObject<T>
	if (const UActorComponent* Comp = Cast<UActorComponent>(GetOuter()))
	{
		return Comp->GetWorld();
	}

	return nullptr;
}

bool UUPAction::CanStart_Implementation(AActor* Instigator) const
{
	if (IsRunning())
	{
		return false;
	}

	UUPActionComponent* Comp = GetOwningComponent();
	if (Comp->ActiveGameplayTags.HasAny(BlockedTags))
	{
		return false;
	}

	return true;
}

UUPActionComponent* UUPAction::GetOwningComponent() const
{
	return Cast<UUPActionComponent>(GetOuter());
}

bool UUPAction::IsRunning_Implementation() const
{
	return bIsRunning;
}
