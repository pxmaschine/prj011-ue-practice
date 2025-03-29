// Fill out your copyright notice in the Description page of Project Settings.

#include "Course/UPGameModeBase.h"
#include "Course/UPAttributeComponent.h"
#include "Course/UPCharacter.h"
#include "Course/UPPlayerState.h"
#include "Course/AI/UPAICharacter.h"
#include "Course/UPMonsterData.h"
#include "Course/UPActionComponent.h"
#include "Course/UPSaveGameSubsystem.h"
#include "Course/UPActorPoolingSubsystem.h"

#include "UEPractice/UEPractice.h"

#include "EngineUtils.h"
#include "Engine/AssetManager.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"


AUPGameModeBase::AUPGameModeBase()
{
	SpawnTimerInterval = 2.0f;
	CreditsPerKill = 20;
	CooldownTimeBetweenFailures = 8.0f;

	PlayerRespawnDelay = 2.0f;

	DesiredPowerupCount = 10;
	RequiredPowerupDistance = 2000;
	InitialSpawnCredit = 50;

	PlayerStateClass = AUPPlayerState::StaticClass();
}

void AUPGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// (Save/Load logic moved into new SaveGameSubsystem)
	UUPSaveGameSubsystem* SG = GetGameInstance()->GetSubsystem<UUPSaveGameSubsystem>();

	// Optional slot name (Falls back to slot specified in SaveGameSettings class/INI otherwise)
	const FString SelectedSaveSlot = UGameplayStatics::ParseOption(Options, "SaveGame");
	SG->LoadSaveGame(SelectedSaveSlot);
}

void AUPGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	// Calling Before Super:: so we set variables before 'beginplayingstate' is called in PlayerController (which is where we instantiate UI)
	UUPSaveGameSubsystem* SG = GetGameInstance()->GetSubsystem<UUPSaveGameSubsystem>();
	SG->HandleStartingNewPlayer(NewPlayer);

	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	// Now we're ready to override spawn location
	// Alternatively we could override core spawn location to use store locations immediately (skipping the whole 'find player start' logic)
	SG->OverrideSpawnTransform(NewPlayer);
}

void AUPGameModeBase::StartPlay()
{
	Super::StartPlay();

	const UUPSaveGameSubsystem* SG = GetGameInstance()->GetSubsystem<UUPSaveGameSubsystem>();
	SG->ApplyLoadedSaveGame();

	AvailableSpawnCredit = InitialSpawnCredit;

	StartSpawningBots();
	
	// Make sure we have assigned at least one power-up class
	if (ensure(PowerupClasses.Num() > 0))
	{
		// Skip the Blueprint wrapper and use the direct C++ option which the Wrapper uses as well
		FEnvQueryRequest Request(PowerupSpawnQuery, this);
		Request.Execute(EEnvQueryRunMode::AllMatching, this, &AUPGameModeBase::OnPowerupSpawnQueryCompleted);
	}
}

void AUPGameModeBase::KillAll()
{
	for (const AUPAICharacter* Bot : TActorRange<AUPAICharacter>(GetWorld()))
	{
		UUPAttributeComponent* AttributeComp = UUPAttributeComponent::GetAttributes(Bot);
		if (ensure(AttributeComp) && AttributeComp->IsAlive())
		{
			AttributeComp->Kill(this);
		}
	}
}

void AUPGameModeBase::OnActorKilled(AActor* VictimActor, AActor* KillerActor)
{
	UE_LOGFMT(LogGame, Log, "OnActorKilled: Victim: {victim}, Killer: {killer}", GetNameSafe(VictimActor), GetNameSafe(KillerActor));

	if (const AUPCharacter* Player = Cast<AUPCharacter>(VictimActor))
	{
		// Auto-respawn
		if (bAutoRespawnPlayer)
		{
			FTimerHandle TimerHandle_RespawnDelay;
			FTimerDelegate Delegate;
			Delegate.BindUObject(this, &ThisClass::RespawnPlayerElapsed, Player->GetController());

			GetWorldTimerManager().SetTimer(TimerHandle_RespawnDelay, Delegate, PlayerRespawnDelay, false);
		}

		// Store time if it was better than previous record
		AUPPlayerState* PS = Player->GetPlayerState<AUPPlayerState>();
		if (PS)
		{
			PS->UpdatePersonalRecord(GetWorld()->TimeSeconds);
		}

		UUPSaveGameSubsystem* SG = GetGameInstance()->GetSubsystem<UUPSaveGameSubsystem>();

		// Immediately auto save on death
		SG->WriteSaveGame();
	}

	// Give Credits for kill
	const APawn* KillerPawn = Cast<APawn>(KillerActor);
	// Don't credit kills of self
	if (KillerPawn && KillerPawn != VictimActor)
	{
		if (AUPPlayerState* PS = KillerPawn->GetPlayerState<AUPPlayerState>()) // < can cast and check for nullptr within if-statement.
		{
			PS->AddCredits(CreditsPerKill);
		}
	}
}

void AUPGameModeBase::StartSpawningBots()
{
	if (TimerHandle_SpawnBots.IsValid())
	{
		// Already spawning bots.
		return;
	}
	
	// Continuous timer to spawn in more bots.
	// Actual amount of bots and whether it's allowed to spawn determined by spawn logic later in the chain...
	GetWorldTimerManager().SetTimer(TimerHandle_SpawnBots, this, &AUPGameModeBase::SpawnBotTimerElapsed, SpawnTimerInterval, true);
}

void AUPGameModeBase::SpawnBotTimerElapsed()
{
	/*
#if WITH_EDITOR
    // disabled as we now use big button in level for debugging, but in normal gameplay something like this is useful
    // does require some code update on how it handles this as 'override' currently not properly set up.
	if (!DevelopmentOnly::bSpawnBotsOverride)
	{
		UE_LOG(LogTemp, Warning, TEXT("Bot spawning disabled via cvar 'CVarSpawnBots'."));
		return;
	}
	#endif*/

	// Give points to spend
	if (SpawnCreditCurve)
	{
		AvailableSpawnCredit += SpawnCreditCurve->GetFloatValue(GetWorld()->TimeSeconds);
	}

	if (CooldownBotSpawnUntil > GetWorld()->TimeSeconds)
	{
		// Still cooling down
		return;
	}

	LogOnScreen(this, FString::Printf(TEXT("Available SpawnCredits: %f"), AvailableSpawnCredit));

	// Count alive bots before spawning
	int32 NrOfAliveBots = 0;
	// TActorRange simplifies the code compared to TActorIterator<T>
	for (const AUPAICharacter* Bot : TActorRange<AUPAICharacter>(GetWorld()))
	{
		const UUPAttributeComponent* AttributeComp = UUPAttributeComponent::GetAttributes(Bot);
		if (ensure(AttributeComp) && AttributeComp->IsAlive())
		{
			NrOfAliveBots++;
		}
	}

	UE_LOGFMT(LogGame, Log, "Found {number} alive bots.", NrOfAliveBots);

	constexpr float MaxBotCount = 40.0f;
	if (NrOfAliveBots >= MaxBotCount)
	{
		UE_LOGFMT(LogGame, Log, "Found {number} alive bots.", NrOfAliveBots);
		return;
	}

	// Row to pass along with EQS delegate
	FMonsterInfoRow* SelectedRow = nullptr;

	// @todo: warn about no monsterrow much earlier in the game and don't even bother arriving here if not set.
	// Use either DataValidation, asserts, or combination to prevent this from crashing here.
	//if (MonsterTable)
	
	TArray<FMonsterInfoRow*> Rows;
	MonsterTable->GetAllRows("", Rows);

	// Get total weight
	float TotalWeight = 0;
	for (const FMonsterInfoRow* Entry : Rows)
	{
		TotalWeight += Entry->Weight;
	}

	// Random number within total random
	const int32 RandomWeight = FMath::RandRange(0.0f, TotalWeight);

	//Reset
	TotalWeight = 0;

	// Get monster based on random weight
	for (FMonsterInfoRow* Entry : Rows)
	{
		TotalWeight += Entry->Weight;

		if (RandomWeight <= TotalWeight)
		{
			SelectedRow = Entry;
			break;
		}
	}

	if (SelectedRow && SelectedRow->SpawnCost >= AvailableSpawnCredit)
	{
		// Too expensive to spawn, try again soon
		CooldownBotSpawnUntil = GetWorld()->TimeSeconds + CooldownTimeBetweenFailures;

		LogOnScreen(this, FString::Printf(TEXT("Cooling down until: %f"), CooldownBotSpawnUntil), FColor::Red);
		return;
	}

	// Skip the Blueprint wrapper and use the direct C++ option which the Wrapper uses as well
	FEnvQueryRequest Request(SpawnBotQuery, this);

	const FQueryFinishedSignature FinishedDelegate = FQueryFinishedSignature::CreateUObject(this, &AUPGameModeBase::OnBotSpawnQueryCompleted, SelectedRow);
	
	Request.Execute(EEnvQueryRunMode::RandomBest5Pct, FinishedDelegate);
}

void AUPGameModeBase::OnBotSpawnQueryCompleted(TSharedPtr<FEnvQueryResult> Result, FMonsterInfoRow* SelectedRow)
{
	FEnvQueryResult* QueryResult = Result.Get();
	if (!QueryResult->IsSuccessful())
	{
		UE_LOGFMT(LogGame, Warning, "Spawn bot EQS Query Failed!");
		return;
	}

	// Retrieve all possible locations that passed the query
	TArray<FVector> Locations;
	QueryResult->GetAllAsLocations(Locations);


	if (Locations.IsValidIndex(0) && MonsterTable)
	{	
		UAssetManager& Manager = UAssetManager::Get();

		// Apply spawn cost
		AvailableSpawnCredit -= SelectedRow->SpawnCost;

		FPrimaryAssetId MonsterId = SelectedRow->MonsterId;

		const TArray<FName> Bundles;
		const FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &AUPGameModeBase::OnMonsterLoaded, MonsterId, Locations[0]);
		Manager.LoadPrimaryAsset(MonsterId, Bundles, Delegate);
	}
}

void AUPGameModeBase::OnMonsterLoaded(FPrimaryAssetId LoadedId, FVector SpawnLocation)
{
	const UAssetManager& Manager = UAssetManager::Get();

	UUPMonsterData* MonsterData = CastChecked<UUPMonsterData>(Manager.GetPrimaryAssetObject(LoadedId));

	// Spawn might fail if colliding with environment
	if (AActor* NewBot = GetWorld()->SpawnActor<AActor>(MonsterData->MonsterClass, SpawnLocation, FRotator::ZeroRotator))
	{
		LogOnScreen(this, FString::Printf(TEXT("Spawned enemy: %s (%s)"), *GetNameSafe(NewBot), *GetNameSafe(MonsterData)));

		// Grant special actions, buffs etc.
		UUPActionComponent* ActionComp = NewBot->FindComponentByClass<UUPActionComponent>();
		check(ActionComp);
		
		for (const TSubclassOf<UUPAction> ActionClass : MonsterData->Actions)
		{
			ActionComp->AddAction(NewBot, ActionClass);
		}
	}
}

void AUPGameModeBase::OnPowerupSpawnQueryCompleted(TSharedPtr<FEnvQueryResult> Result)
{
	FEnvQueryResult* QueryResult = Result.Get();
	if (!QueryResult->IsSuccessful())
	{
		UE_LOGFMT(LogGame, Warning, "Spawn bot EQS Query Failed!");
		return;
	}

	// Retrieve all possible locations that passed the query
	TArray<FVector> Locations;
	QueryResult->GetAllAsLocations(Locations);

	// Keep used locations to easily check distance between points
	TArray<FVector> UsedLocations;
	int32 SpawnCounter = 0;

	// Break out if we reached the desired count or if we have no more potential positions remaining
	while (SpawnCounter < DesiredPowerupCount && Locations.Num() > 0)
	{
		// Pick a random location from remaining points.
		int32 RandomLocationIndex = FMath::RandRange(0, Locations.Num() - 1);
		FVector PickedLocation = Locations[RandomLocationIndex];

		// Remove to avoid picking again
		Locations.RemoveAt(RandomLocationIndex);

		// Check minimum distance requirement
		bool bValidLocation = true;

		for (FVector OtherLocation : UsedLocations)
		{
			float DistanceTo = (PickedLocation - OtherLocation).Size();
			if (DistanceTo < RequiredPowerupDistance)
			{
				// Show skipped locations due to distance
				//DrawDebugSphere(GetWorld(), PickedLocation, 50.0f, 20, FColor::Red, false, 10.0f);
				// too close, skip to next attempt
				bValidLocation = false;
				break;
			}
		}

		// Failed the distance test
		if (!bValidLocation)
		{
			continue;
		}

		// Pick a random powerup-class
		const int32 RandomClassIndex = FMath::RandRange(0, PowerupClasses.Num() - 1);
		TSubclassOf<AActor> RandomPowerupClass = PowerupClasses[RandomClassIndex];
		GetWorld()->SpawnActor<AActor>(RandomPowerupClass, PickedLocation, FRotator::ZeroRotator);

		// Keep for distance checks
		UsedLocations.Add(PickedLocation);
		SpawnCounter++;
	}
}

void AUPGameModeBase::RespawnPlayerElapsed(AController* Controller)
{
	if (ensure(Controller))
	{
		Controller->UnPossess();

		RestartPlayer(Controller);
	}
}

void AUPGameModeBase::RequestPrimedActors()
{
	UUPActorPoolingSubsystem* PoolingSystem = GetWorld()->GetSubsystem<UUPActorPoolingSubsystem>();
	for (const auto& Entry : ActorPoolClasses)
	{
		PoolingSystem->PrimeActorPool(Entry.Key, Entry.Value);
	}
}
