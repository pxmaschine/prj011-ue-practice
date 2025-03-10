// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPAction.h"
#include "Course/UPActionComponent.h"

#include "Net/UnrealNetwork.h"
#include "UEPractice/UEPractice.h"


void UUPAction::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UUPAction, RepData);
	DOREPLIFETIME(UUPAction, ActionComp);
}

void UUPAction::Initialize(UUPActionComponent* NewActionComp)
{
	ActionComp = NewActionComp;
}

void UUPAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Running: %s"), *GetNameSafe(this));
	//LogOnScreen(this, FString::Printf(TEXT("Started: %s"), *ActionName.ToString()), FColor::Green);

	UUPActionComponent* Comp = GetOwningComponent();
	Comp->ActiveGameplayTags.AppendTags(GrantedTags);

	RepData.bIsRunning = true;
	RepData.Instigator = Instigator;
}

void UUPAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Stopped: %s"), *GetNameSafe(this));
	//LogOnScreen(this, FString::Printf(TEXT("Stopped: %s"), *ActionName.ToString()), FColor::White);

	// TODO: Not true for the client
	//ensureAlways(bIsRunning);

	UUPActionComponent* Comp = GetOwningComponent();
	Comp->ActiveGameplayTags.RemoveTags(GrantedTags);

	RepData.bIsRunning = false;
	RepData.Instigator = Instigator;
}

UWorld* UUPAction::GetWorld() const
{
	// Outer is set when creating action via NewObject<T>
	if (const AActor* Actor = Cast<AActor>(GetOuter()))
	{
		return Actor->GetWorld();
	}

	return nullptr;
}

bool UUPAction::CanStart_Implementation(AActor* Instigator) const
{
	if (IsRunning())
	{
		return false;
	}

	const UUPActionComponent* Comp = GetOwningComponent();
	if (Comp->ActiveGameplayTags.HasAny(BlockedTags))
	{
		return false;
	}

	return true;
}

UUPActionComponent* UUPAction::GetOwningComponent() const
{
	return ActionComp;
}

void UUPAction::OnRep_RepData()
{
	if (RepData.bIsRunning)
	{
		StartAction(RepData.Instigator);
	}
	else
	{
		StopAction(RepData.Instigator);
	}
}

bool UUPAction::IsRunning_Implementation() const
{
	return RepData.bIsRunning;
}
