// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPActionComponent.h"
#include "Course/UPAction.h"
#include "UEPractice/UEPractice.h"

#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"


DECLARE_CYCLE_STAT(TEXT("StartActionByName"), STAT_StartActionByName, STATGROUP_UEPRACTICE);

UUPActionComponent::UUPActionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UUPActionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UUPActionComponent, Actions);
}

void UUPActionComponent::BeginPlay()
{
	Super::BeginPlay();

	// Server only
	if (GetOwner()->HasAuthority())
	{
		for (const TSubclassOf<UUPAction> ActionClass : DefaultActions)
		{
			AddAction(GetOwner(), ActionClass);
		}	
	}
}

void UUPActionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	TArray ActionsCopy(Actions);
	for (UUPAction* Action : ActionsCopy)
	{
		if (Action && Action->IsRunning())
		{
			Action->StopAction(GetOwner());
		}
	}

	Super::EndPlay(EndPlayReason);
}

void UUPActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//const FString DebugMsg = GetNameSafe(GetOwner()) + " : " + ActiveGameplayTags.ToStringSimple( );
	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::White, DebugMsg);

	// Draw all actions
	for (const UUPAction* Action : Actions)
	{
		const FColor TextColor = Action->IsRunning() ? FColor::Blue : FColor::White;
		FString ActionMsg = FString::Printf(TEXT("[%s] Action: %s"), *GetNameSafe(GetOwner()), *GetNameSafe(Action));
		LogOnScreen(this, ActionMsg, TextColor, 0.0f);
	}
}

bool UUPActionComponent::ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (UUPAction* Action : Actions)
	{
		if (Action)
		{
			WroteSomething |= Channel->ReplicateSubobject(Action, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

void UUPActionComponent::AddAction(AActor* Instigator, TSubclassOf<UUPAction> ActionClass)
{
	if (!ensure(ActionClass))
	{
		return;
	}

	// Skip for clients
	if (!GetOwner()->HasAuthority())
	{
		UE_LOGFMT(LogGame, Warning, "Client attempting to AddAction. [Class: {Class}]", GetNameSafe(ActionClass));
		return;
	}

	UUPAction* NewAction = NewObject<UUPAction>(GetOwner(), ActionClass);
	if (ensure(NewAction))
	{
		NewAction->Initialize(this);

		Actions.Add(NewAction);

		if (NewAction->bAutoStart && ensure(NewAction->CanStart(Instigator)))
		{
			NewAction->StartAction(Instigator);
		}
	}
}

void UUPActionComponent::RemoveAction(UUPAction* Action)
{
	if (!ensure(Action && !Action->IsRunning()))
	{
		return;
	}

	Actions.Remove(Action);
}

UUPAction* UUPActionComponent::GetAction(TSubclassOf<UUPAction> ActionClass) const
{
	for (UUPAction* Action : Actions)
	{
 		if (Action && Action->IsA(ActionClass))
 		{
 			return Action;
 		}
	}

	return nullptr;
}

bool UUPActionComponent::StartActionByName(AActor* Instigator, FGameplayTag ActionName)
{
	SCOPE_CYCLE_COUNTER(STAT_StartActionByName);

	for (UUPAction* Action : Actions)
	{
		if (Action && Action->ActivationTag == ActionName)
		{
			if (!Action->CanStart(Instigator))
			{
				const FString DebugMsg = FString::Printf(TEXT("Failed to run: %s"), *ActionName.ToString());
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, DebugMsg);
				continue;
			}

			// Is client?
			if (!GetOwner()->HasAuthority())
			{
				ServerStartAction(Instigator, ActionName);
			}

			// Bookmark for Unreal Insights
			TRACE_BOOKMARK(TEXT("StartAction::%s"), *GetNameSafe(Action))

			Action->StartAction(Instigator);
			return true;
		}
	}

	return false;
}

bool UUPActionComponent::StopActionByName(AActor* Instigator, FGameplayTag ActionName)
{
	for (UUPAction* Action : Actions)
	{
		if (Action && Action->ActivationTag == ActionName)
		{
			if (Action->IsRunning())
			{
				// Is client?
				if (!GetOwner()->HasAuthority())
				{
					ServerStopAction(Instigator, ActionName);
				}

				Action->StopAction(Instigator);
				return true;
			}
		}
	}

	return false;
}

void UUPActionComponent::ServerStartAction_Implementation(AActor* Instigator, FGameplayTag ActionName)
{
	StartActionByName(Instigator, ActionName);
}

void UUPActionComponent::ServerStopAction_Implementation(AActor* Instigator, FGameplayTag ActionName)
{
	StopActionByName(Instigator, ActionName);
}
