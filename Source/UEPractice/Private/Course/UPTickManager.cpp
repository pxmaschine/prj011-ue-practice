// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPTickManager.h"
#include "UEPractice/UEPractice.h"


static TAutoConsoleVariable CVarAggregateTicks(
	TEXT("game.AggregateTicks"),
	true,
	TEXT("Enable aggregate ticking for selected objects. Takes effect on next level load."),
	ECVF_Default);

void FTickablesTickFunction::ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread,
	const FGraphEventRef& MyCompletionGraphEvent)
{
	Target->ExecuteTick(TickGroup, DeltaTime, TickType, CurrentThread, MyCompletionGraphEvent);
}

void UUPTickManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	// Register for once to tick all components
	// @todo: expand to have one option per tick group

	if (CVarAggregateTicks.GetValueOnGameThread())
	{
		ComponentsTick.bCanEverTick = true;
        ComponentsTick.Target = this;
		ComponentsTick.TickGroup = TG_PrePhysics;
        
        // interesting to try async logic
        ComponentsTick.bRunOnAnyThread = false;
        	
        ComponentsTick.RegisterTickFunction(GetWorld()->PersistentLevel);
	}
}

void UUPTickManager::RegisterComponent(FActorComponentTickFunction* TickFunction)
{
	if (CVarAggregateTicks.GetValueOnGameThread())
	{
		// Remove from the standard system
		TickFunction->UnRegisterTickFunction();

		check(!TickableComponents.Contains(TickFunction));
		TickableComponents.Add(TickFunction);
	}
}

void UUPTickManager::UnregisterComponent(FActorComponentTickFunction* TickFunction)
{
	if (CVarAggregateTicks.GetValueOnGameThread())
	{
		// Cleanup
		CleanupQueue.Add(TickFunction);
	}
}

void UUPTickManager::ExecuteTick(ETickingGroup TickGroup, float DeltaTime, ELevelTick TickType,
	ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent)
{
	// "Old" Stats system
	DECLARE_SCOPE_CYCLE_COUNTER(TEXT("AggregateComponentTick"), AggregateComponentTick, STATGROUP_UEPRACTICE)

	{
		SCOPED_NAMED_EVENT(TickManagedComponents, FColor::Orange);
	
		// @todo: want one of these loops for every class to keep them sorted (otherwise this entire concept of "aggregating ticks" fails)
		// In the example only 1 type of components is ticked here. For multiple types you should store them as discrete lists or sorted
		for (FActorComponentTickFunction* Func : TickableComponents)
		{
			Func->ExecuteTick(DeltaTime, TickType, CurrentThread, MyCompletionGraphEvent);
		}
	}

	// Cleanup after ticking all components
	for (FActorComponentTickFunction* Func : CleanupQueue)
	{
		TickableComponents.Remove(Func);
	}
	CleanupQueue.Empty();
}
