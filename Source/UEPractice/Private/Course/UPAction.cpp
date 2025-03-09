// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPAction.h"
#include "Course/UPActionComponent.h"

#include "Net/UnrealNetwork.h"
#include "UEPractice/UEPractice.h"


void UUPAction::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UUPAction, bIsRunning);
	DOREPLIFETIME(UUPAction, ActionComp);
}

void UUPAction::Initialize(UUPActionComponent* NewActionComp)
{
	ActionComp = NewActionComp;
}

void UUPAction::StartAction_Implementation(AActor* Instigator)
{
	//UE_LOG(LogTemp, Log, TEXT("Running: %s"), *GetNameSafe(this));
	LogOnScreen(this, FString::Printf(TEXT("Started: %s"), *ActionName.ToString()), FColor::Green);

	UUPActionComponent* Comp = GetOwningComponent();
	Comp->ActiveGameplayTags.AppendTags(GrantedTags);

	bIsRunning = true;
}

void UUPAction::StopAction_Implementation(AActor* Instigator)
{
	//UE_LOG(LogTemp, Log, TEXT("Stopped: %s"), *GetNameSafe(this));
	LogOnScreen(this, FString::Printf(TEXT("Stopped: %s"), *ActionName.ToString()), FColor::White);

	// TODO: Not true for the client
	//ensureAlways(bIsRunning);

	UUPActionComponent* Comp = GetOwningComponent();
	Comp->ActiveGameplayTags.RemoveTags(GrantedTags);

	bIsRunning = false;
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

	UUPActionComponent* Comp = GetOwningComponent();
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

void UUPAction::OnRep_IsRunning()
{
	if (bIsRunning)
	{
		StartAction(nullptr);
	}
	else
	{
		StopAction(nullptr);
	}
}

bool UUPAction::IsRunning_Implementation() const
{
	return bIsRunning;
}
