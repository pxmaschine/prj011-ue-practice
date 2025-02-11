// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPGameModeBase.h"

#include "EngineUtils.h"
#include "Course/AI/UPAICharacter.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryTypes.h"

AUPGameModeBase::AUPGameModeBase()
{
	SpawnTimerInterval = 2.0f;
}

void AUPGameModeBase::StartPlay()
{
	Super::StartPlay();

	// Continuous timer to spawn in more bots
	// Actual amount of bots and whether its allowed to spawn determined by spawn logic later in the chain
	GetWorldTimerManager().SetTimer(TimerHandle_SpawnBots, this, &AUPGameModeBase::SpawnBotTimerElapsed, SpawnTimerInterval, true);
}

void AUPGameModeBase::SpawnBotTimerElapsed()
{
	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, SpawnBotQuery, this, EEnvQueryRunMode::RandomBest25Pct, nullptr);
	if (ensure(QueryInstance))
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &AUPGameModeBase::OnQueryCompleted);
	}
}

void AUPGameModeBase::OnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance,
	EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn bot EQS Query failed!"));
		return;
	}

	int32 NrOfAliveBots = 0;
	for (TActorIterator<AUPAICharacter> It(GetWorld()); It; ++It)
	{
		const AUPAICharacter* Bot = *It;

		const UUPAttributeComponent* AttributeComp = Cast<UUPAttributeComponent>(Bot->GetComponentByClass(UUPAttributeComponent::StaticClass()));
		if (AttributeComp && AttributeComp->IsAlive())
		{
			NrOfAliveBots++;
		}
	}

	int32 MaxBotCount = 3;

	if (DifficultyCurve)
	{
		MaxBotCount = DifficultyCurve->GetFloatValue(GetWorld()->TimeSeconds);
	}

	if (NrOfAliveBots >= MaxBotCount)
	{
		return;
	}

	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();

	if (Locations.Num() > 0)
	{
		GetWorld()->SpawnActor<AActor>(MinionClass, Locations[0], FRotator::ZeroRotator);
	}
}
