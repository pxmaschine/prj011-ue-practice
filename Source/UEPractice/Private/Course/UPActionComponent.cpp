// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPActionComponent.h"
#include "Course/UPAction.h"
#include "Course/UPGameplayInterface.h"
#include "UEPractice/UEPractice.h"

#include "Net/UnrealNetwork.h"


UUPActionComponent::UUPActionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	// See GDefaultUseSubObjectReplicationList for CVAR to enable by default project-wide
	bReplicateUsingRegisteredSubObjectList = true;
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
	// Stop all
	TArray<UUPAction*> ActionsCopy = Actions;
	for (UUPAction* Action : ActionsCopy)
	{
		if (Action->IsRunning())
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

	//// Draw all actions
	//for (const UUPAction* Action : Actions)
	//{
	//	const FColor TextColor = Action->IsRunning() ? FColor::Blue : FColor::White;
	//	FString ActionMsg = FString::Printf(TEXT("[%s] Action: %s"), *GetNameSafe(GetOwner()), *GetNameSafe(Action));
	//	LogOnScreen(this, ActionMsg, TextColor, 0.0f);
	//}
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
	check(NewAction);

	NewAction->Initialize(this);

	Actions.Add(NewAction);

	// New Replicated Objects list (for networking)
	AddReplicatedSubObject(NewAction);

	if (NewAction->IsAutoStart() && ensure(NewAction->CanStart(Instigator)))
	{
		NewAction->StartAction(Instigator);
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
		if (Action->IsA(ActionClass))
 		{
 			return Action;
 		}
	}

	return nullptr;
}

bool UUPActionComponent::StartActionByName(AActor* Instigator, FGameplayTag ActionName)
{
	// Visible in Unreal Insights with namedevents enabled
	SCOPED_NAMED_EVENT(StartActionName, FColor::Green);
	// Alternative, available when cpu channel is specified
	//TRACE_CPUPROFILER_EVENT_SCOPE(StartActionByName);

	for (UUPAction* Action : Actions)
	{
		if (Action->GetActivationTag() == ActionName)
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
		if (Action->GetActivationTag() == ActionName)
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

UUPActionComponent* UUPActionComponent::GetComponent(AActor* InActor)
{
	if (InActor && InActor->Implements<UUPGameplayInterface>())
	{
		UUPActionComponent* ActionComp = nullptr;
		if (IUPGameplayInterface::Execute_GetActionComponent(InActor, ActionComp))
		{
			return ActionComp;
		}
	}

	// @todo: log warn about interface not implemented yet

	// Iterate over all components anyway if not implemented. But warn about this

	return InActor->GetComponentByClass<UUPActionComponent>();
}
