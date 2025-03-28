// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPActorPoolingSubsystem.h"
#include "Course/UPActorPoolingInterface.h"

#include "UEPractice/UEPractice.h"


static TAutoConsoleVariable CVarActorPoolingEnabled(
	TEXT("up.ActorPooling"),
	true,
	TEXT("Enable actor pooling for selected objects."),
	ECVF_Default);


AActor* UUPActorPoolingSubsystem::SpawnActorPooled(const UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, const FTransform& SpawnTransform, ESpawnActorCollisionHandlingMethod SpawnHandling)
{
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = SpawnHandling;
	
	return AcquireFromPool(WorldContextObject, ActorClass, SpawnTransform, Params);
}

bool UUPActorPoolingSubsystem::ReleaseToPool(AActor* Actor)
{
	if (IsPoolingEnabled(Actor))
	{
		UUPActorPoolingSubsystem* PoolingSubsystem = Actor->GetWorld()->GetSubsystem<UUPActorPoolingSubsystem>();
		return PoolingSubsystem->ReleaseToPool_Internal(Actor);
	}

	SCOPED_NAMED_EVENT(DestroyActorNoPool, FColor::Red);
	Actor->Destroy();
	return false;
}

AActor* UUPActorPoolingSubsystem::AcquireFromPool(const UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, const FTransform& SpawnTransform, FActorSpawnParameters SpawnParams)
{
	if (IsPoolingEnabled(WorldContextObject))
	{
		UUPActorPoolingSubsystem* PoolingSystem = WorldContextObject->GetWorld()->GetSubsystem<UUPActorPoolingSubsystem>();
		return PoolingSystem->AcquireFromPool_Internal(ActorClass, SpawnTransform, SpawnParams);
	}
	
	SCOPED_NAMED_EVENT(SpawnActorNoPool, FColor::Red);
	// Fallback to standard spawning when not enabled
	return WorldContextObject->GetWorld()->SpawnActor<AActor>(ActorClass, SpawnTransform, SpawnParams);
}

bool UUPActorPoolingSubsystem::IsPoolingEnabled(const UObject* WorldContextObject)
{
	return CVarActorPoolingEnabled.GetValueOnAnyThread() && WorldContextObject->GetWorld()->IsNetMode(NM_Standalone);
}
	
void UUPActorPoolingSubsystem::PrimeActorPool(TSubclassOf<AActor> ActorClass, int32 Amount)
{
	UE_LOGFMT(LogGame, Log, "Priming Pool for {actorclass} ({amount})", GetNameSafe(ActorClass), Amount);
	SCOPED_NAMED_EVENT(PrimeActorPool, FColor::Blue);

	// Prime a set number of pooled actors, this reduces memory fragmentation and any potential initial hitches during gameplay
	for (int i = 0; i < Amount; ++i)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		AActor* NewActor = GetWorld()->SpawnActor<AActor>(ActorClass, FTransform::Identity, Params);

		ReleaseToPool(NewActor);
	}
}

bool UUPActorPoolingSubsystem::ReleaseToPool_Internal(AActor* Actor)
{
	SCOPED_NAMED_EVENT(ReleaseActorToPool, FColor::White);
	check(IsValid(Actor));

	// These are assumed not used by game code
	Actor->SetActorEnableCollision(false);
	Actor->SetActorHiddenInGame(true);

	Actor->RouteEndPlay(EEndPlayReason::Destroyed);

	IUPActorPoolingInterface::Execute_PoolEndPlay(Actor);

	// de-register any tick functions

	// Place in the pool for later use
	FActorPool* ActorPool = &AvailableActorPool.FindOrAdd(Actor->GetClass());
	ActorPool->FreeActors.Add(Actor);

	return true;
}

AActor* UUPActorPoolingSubsystem::AcquireFromPool_Internal(TSubclassOf<AActor> ActorClass, const FTransform& SpawnTransform, FActorSpawnParameters SpawnParams)
{
	SCOPED_NAMED_EVENT(AcquireActorFromPool, FColor::White);
	
	AActor* AcquiredActor = nullptr;

	FActorPool* ActorPool = &AvailableActorPool.FindOrAdd(ActorClass);
	// Grab first available
	if (ActorPool->FreeActors.IsValidIndex(0))
	{
		UE_LOGFMT(LogGame, Log, "Acquired Actor for {actorclass} from pool", GetNameSafe(ActorClass));
		
		AcquiredActor = ActorPool->FreeActors[0];

		// Remove from pool
		ActorPool->FreeActors.RemoveAt(0, 1, false);
	}

	// Failed to find actor
	if (AcquiredActor == nullptr)
	{
		UE_LOGFMT(LogGame, Log, "Actor Pool empty, spawning new Actor for {actorclass}", GetNameSafe(ActorClass));
		
		// Spawn fresh instance that can eventually be release to the pool
		return GetWorld()->SpawnActor<AActor>(ActorClass, SpawnTransform, SpawnParams);
	}

	AcquiredActor->SetActorTransform(SpawnTransform);
	AcquiredActor->SetInstigator(SpawnParams.Instigator);
	AcquiredActor->SetOwner(SpawnParams.Owner);

	// These are assumed not used by game code
	AcquiredActor->SetActorEnableCollision(true);
	AcquiredActor->SetActorHiddenInGame(false);

	AcquiredActor->DispatchBeginPlay();

	IUPActorPoolingInterface::Execute_PoolBeginPlay(AcquiredActor);

	return AcquiredActor;
}
