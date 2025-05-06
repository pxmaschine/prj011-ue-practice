// Fill out your copyright notice in the Description page of Project Settings.


#include <Game/GGameModeBase.h>
#include <Course/UPDeveloperSettings.h>
#include <UEPractice/UEPractice.h>
#include <EnvironmentQuery/EnvQueryManager.h>

AGGameModeBase::AGGameModeBase()
{
	SpawnTimerInterval = 10.0f;
	MaxEnemyCount = 10;
}

void AGGameModeBase::StartPlay()
{
	Super::StartPlay();

	StartSpawningEnemies();
}

void AGGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearAllTimersForObject(this);

	Super::EndPlay(EndPlayReason);
}

void AGGameModeBase::OnEnemyKilled(AActor* EnemyActor)
{
	ActiveEnemies.Remove(EnemyActor);
}

void AGGameModeBase::StartSpawningEnemies()
{
	if (TimerHandle_SpawnEnemy.IsValid())
	{
		// Already spawning bots.
		return;
	}
	
	// Continuous timer to spawn in more bots.
	// Actual amount of bots and whether it's allowed to spawn determined by spawn logic later in the chain...
	GetWorldTimerManager().SetTimer(TimerHandle_SpawnEnemy, this, &AGGameModeBase::SpawnEnemyTimerElapsed, SpawnTimerInterval, true);
}

void AGGameModeBase::SpawnEnemyTimerElapsed()
{
#if !UE_BUILD_SHIPPING
	const UUPDeveloperSettings* DevSettings = GetDefault<UUPDeveloperSettings>();
	if (DevSettings->DisableSpawnBotsOverride())
	{
		return;
	}
#endif

	UE_LOGFMT(LogGame, Log, "Found {number} alive enemies.", ActiveEnemies.Num());

	if (ActiveEnemies.Num() >= MaxEnemyCount)
	{
		UE_LOGFMT(LogGame, Log, "Max number of enemies reached.");
		return;
	}

	// Skip the Blueprint wrapper and use the direct C++ option which the Wrapper uses as well
	FEnvQueryRequest Request(SpawnEnemyQuery, this);
	const FQueryFinishedSignature FinishedDelegate = FQueryFinishedSignature::CreateUObject(this, &AGGameModeBase::OnEnemySpawnQueryCompleted);
	Request.Execute(EEnvQueryRunMode::RandomBest5Pct, FinishedDelegate);
}

void AGGameModeBase::OnEnemySpawnQueryCompleted(TSharedPtr<FEnvQueryResult> Result)
{
	FEnvQueryResult* QueryResult = Result.Get();
	if (!QueryResult->IsSuccessful())
	{
		UE_LOGFMT(LogGame, Warning, "Spawn enemy EQS Query Failed!");
		return;
	}

	// Retrieve all possible locations that passed the query
	TArray<FVector> Locations;
	QueryResult->GetAllAsLocations(Locations);

	if (Locations.IsEmpty())
	{
		UE_LOGFMT(LogGame, Warning, "No spawn location found!");
		return;
	}

	// Spawn might fail if colliding with environment
	if (AActor* NewEnemy = GetWorld()->SpawnActor<AActor>(EnemyClass, Locations[0], FRotator::ZeroRotator))
	{
		LogOnScreen(this, FString::Printf(TEXT("Spawned enemy: %s (%s)"), *GetNameSafe(NewEnemy), *GetNameSafe(EnemyClass)));

		ActiveEnemies.Add(NewEnemy);
	}
}
