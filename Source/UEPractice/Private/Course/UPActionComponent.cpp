// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPActionComponent.h"
#include "Course/UPAction.h"
#include "Course/UPGameplayFunctionLibrary.h"
#include "UEPractice/UEPractice.h"

#include "Net/UnrealNetwork.h"


UUPActionComponent::UUPActionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;

	SetIsReplicatedByDefault(true);

	// See GDefaultUseSubObjectReplicationList for CVAR to enable by default project-wide
	bReplicateUsingRegisteredSubObjectList = true;
}

void UUPActionComponent::InitializeComponent()
{
	Super::InitializeComponent();

	{
		TRACE_CPUPROFILER_EVENT_SCOPE(CacheAllAttributes);

		for (TFieldIterator<FStructProperty> PropertyIt(AttributeSet.GetScriptStruct()); PropertyIt; ++PropertyIt)
		{
			const FUPAttribute* FoundAttribute = PropertyIt->ContainerPtrToValuePtr<FUPAttribute>(AttributeSet.GetMemory());

			// Build the tag "Attribute.Health" where "Health" is the variable name of the RogueAttribute we just iterated
			FString TagName = TEXT("Attribute." + PropertyIt->GetName());
			FGameplayTag AttributeTag = FGameplayTag::RequestGameplayTag(FName(TagName));

			AttributeCache.Add(AttributeTag, const_cast<FUPAttribute*>(FoundAttribute));
		}
	}
}

void UUPActionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UUPActionComponent, AttributeSet);
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
	StopAllActions();

	Super::EndPlay(EndPlayReason);
}

UUPActionComponent* UUPActionComponent::GetActionComponent(AActor* FromActor)
{
	return UUPGameplayFunctionLibrary::GetActionComponentFromActor(FromActor);
}

FUPAttribute* UUPActionComponent::GetAttribute(FGameplayTag InAttributeTag)
{
	if (FUPAttribute** FoundAttribute = AttributeCache.Find(InAttributeTag))
	{
		return *FoundAttribute;
	}

	return nullptr;
}

bool UUPActionComponent::ApplyAttributeChange(const FAttributeModification& Modification)
{
	FUPAttribute* Attribute = GetAttribute(Modification.AttributeTag);

	float OriginalValue = Attribute->GetValue();

	switch (Modification.ModifyType)
	{
		case EAttributeModifyType::AddBase:
			{
				Attribute->Base += Modification.Magnitude;
				break;
			}
		case EAttributeModifyType::AddDelta:
			{
				Attribute->Delta += Modification.Magnitude;
				break;
			}
		case EAttributeModifyType::OverrideBase:
			{
				Attribute->Base = Modification.Magnitude;
				break;
			}
		default:
			// Always fail here so we can address it
			check(false);
	}

	// With clamping inside the attribute (or a zero delta) no real change might have occured
	if (!FMath::IsNearlyEqual(OriginalValue, Attribute->GetValue()))
	{
		Attribute->OnAttributeChanged.Broadcast(Attribute->GetValue(), Modification);
		return true;
	}
	
	// no actual change occured
	return false;
}

void UUPActionComponent::K2_AddAttributeListener(FGameplayTag AttributeTag, FOnAttributeChangedDynamic Event, bool bCallImmediately /*= false*/)
{
	//FAttributeDelegateHandle Wrapper;
	//Wrapper.DynamicDelegate = Event;
	//AttributeListeners.Add(TPair<FGameplayTag, FAttributeDelegateHandle>(AttributeTag, Wrapper));

	if (!AttributeTag.IsValid())
	{
		UE_LOG(LogGame, Log, TEXT("No valid GameplayTag specified in AddAttributeListener for %s"), *GetNameSafe(GetOwner()));
		return;
	}

	FUPAttribute* FoundAttribute = GetAttribute(AttributeTag);

	// An unusual "Wrapper" to make the binding easier in blueprint (returns handle to unbind from Blueprint if needed)
	FDelegateHandle Handle = FoundAttribute->OnAttributeChanged.AddLambda([Event, FoundAttribute, this](float NewValue, FAttributeModification AttriMod)
	{
		bool bIsBound = Event.ExecuteIfBound(NewValue, AttriMod);

		// We instance was deleted, the event is no longer valid
		if (!bIsBound)
		{
			FDelegateHandle Handle = *DynamicDelegateHandles.Find(Event);
			FoundAttribute->OnAttributeChanged.Remove(Handle);
		}
	});

	// Keep track so it can be cleaned up if blueprint owning is deleted
	DynamicDelegateHandles.Add(Event, Handle);

	// Calling immediately is convenient for setting up initial states like in UI
	if (bCallImmediately)
	{
		// @todo: maybe change EAttributeModifyType?
		const FAttributeModification AttriMod = FAttributeModification(AttributeTag, 0.0f, this, GetOwner(), EAttributeModifyType::Invalid);
		
		Event.Execute(FoundAttribute->GetValue(), AttriMod);
	}
}

bool UUPActionComponent::K2_GetAttribute(FGameplayTag InAttributeTag, float& CurrentValue, float& Base, float& Delta)
{
	if (const FUPAttribute* FoundAttribute = GetAttribute(InAttributeTag))
	{
		CurrentValue = FoundAttribute->GetValue();
		Base = FoundAttribute->Base;
		Delta = FoundAttribute->Delta;
	}

	return false;
}

void UUPActionComponent::SetDefaultAttributeSet(UScriptStruct* InDefaultType)
{
	// @todo: maybe add safeguards to only allow this during init. We don't want to swap out set during gameplay
	
	AttributeSet = FInstancedStruct(InDefaultType);
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

	RemoveReplicatedSubObject(Action);

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
			//TRACE_BOOKMARK(TEXT("StartAction::%s"), *GetNameSafe(Action))

			{
				// Scoped within the curly braces. the _FSTRING variant adds additional tracing overhead due to grabbing the class name every time
				SCOPED_NAMED_EVENT_FSTRING(Action->GetClass()->GetName(), FColor::White);

				Action->StartAction(Instigator);
			}

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

void UUPActionComponent::StopAllActions()
{
	TArray<UUPAction*> ActionsCopy = Actions;
	for (UUPAction* Action : ActionsCopy)
	{
		if (Action->IsRunning())
		{
			Action->StopAction(GetOwner());
		}
	}
}

void UUPActionComponent::ServerStartAction_Implementation(AActor* Instigator, FGameplayTag ActionName)
{
	StartActionByName(Instigator, ActionName);
}

void UUPActionComponent::ServerStopAction_Implementation(AActor* Instigator, FGameplayTag ActionName)
{
	StopActionByName(Instigator, ActionName);
}
