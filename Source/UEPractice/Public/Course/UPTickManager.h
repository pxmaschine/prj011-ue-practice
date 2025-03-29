// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "UPTickManager.generated.h"


class UUPTickManager;

/* Registered tick of the subsystem with the standard tick task graph */
USTRUCT()
struct FTickablesTickFunction : public FTickFunction
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	UUPTickManager* Target = nullptr;

	virtual void ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent) override;

	virtual FString DiagnosticMessage() override {return TEXT("FAggregateTickFunction"); }
};

template <>
struct TStructOpsTypeTraits<FTickablesTickFunction> : TStructOpsTypeTraitsBase2<FTickablesTickFunction>
{
	enum
	{
		WithCopy = false
	};
};

UCLASS()
class UEPRACTICE_API UUPTickManager : public UWorldSubsystem
{
	GENERATED_BODY()

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

public:
	void RegisterComponent(FActorComponentTickFunction* TickFunction);
	
	void UnregisterComponent(FActorComponentTickFunction* TickFunction);
	
	void ExecuteTick(ETickingGroup TickGroup, float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent);
	
protected:
	TArray<FActorComponentTickFunction*> TickableComponents;

	FTickablesTickFunction ComponentsTick;

	/* Cleanup after tick to avoid asserts when we get destroyed during iteration */
	TArray<FActorComponentTickFunction*> CleanupQueue;
};
