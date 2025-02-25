// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPAction.h"

void UUPAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Running: %s"), *GetNameSafe(this));
}

void UUPAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Stopped: %s"), *GetNameSafe(this));
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
