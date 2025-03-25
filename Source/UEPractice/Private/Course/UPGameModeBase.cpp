// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPGameModeBase.h"
#include "Course/UPAttributeComponent.h"
#include "Course/UPCharacter.h"
#include "Course/UPPlayerState.h"
#include "Course/AI/UPAICharacter.h"
#include "Course/UPSaveGame.h"
#include "Course/UPMonsterData.h"
#include "Course/UPActionComponent.h"
#include "Course/UPGameplayInterface.h"
#include "Course/UPSaveGameSubsystem.h"

#include "EngineUtils.h"
#include "Engine/AssetManager.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "UEPractice/UEPractice.h"


static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("up_SpawnBots"), true, TEXT("Enable spawning of bots via timer."), ECVF_Cheat);


AUPGameModeBase::AUPGameModeBase()
{
	SpawnTimerInterval = 2.0f;
	PlayerRespawnDelay = 2.0f;

	CreditsPerKill = 20;
	DesiredPowerupCount = 10;
	RequiredPowerupDistance = 2000;

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

	// Continuous timer to spawn in more bots
	// Actual amount of bots and whether its allowed to spawn determined by spawn logic later in the chain
	GetWorldTimerManager().SetTimer(TimerHandle_SpawnBots, this, &AUPGameModeBase::SpawnBotTimerElapsed, SpawnTimerInterval, true);

	// Make sure we have assigned at least one power-up class
	if (ensure(PowerupClasses.Num() > 0))
	{
		// Run EQS to find potential power-up spawn locations
		UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, PowerupSpawnQuery, this, EEnvQueryRunMode::AllMatching, nullptr);
		if (ensure(QueryInstance))
		{
			QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &AUPGameModeBase::OnPowerupSpawnQueryCompleted);
		}
	}
}

void AUPGameModeBase::KillAll()
{
	for (TActorIterator<AUPAICharacter> It(GetWorld()); It; ++It)
	{
		const AUPAICharacter* Bot = *It;

		UUPAttributeComponent* AttributeComp = UUPAttributeComponent::GetAttributes(Bot);
		if (ensure(AttributeComp) && AttributeComp->IsAlive())
		{
			AttributeComp->Kill(this);
		}
	}
}

void AUPGameModeBase::OnActorKilled(AActor* VictimActor, AActor* KillerActor)
{
	UE_LOG(LogTemp, Log, TEXT("OnActorKilled: Victim: %s, Killer: %s"), *GetNameSafe(VictimActor), *GetNameSafe(KillerActor));

	if (const AUPCharacter* Player = Cast<AUPCharacter>(VictimActor))
	{
		// Disable auto-respawn
		//FTimerHandle TimerHandle_RespawnDelay;
		//FTimerDelegate Delegate;
		//Delegate.BindUFunction(this, "RespawnPlayerElapsed", Player->GetController());

		//GetWorldTimerManager().SetTimer(TimerHandle_RespawnDelay, Delegate, PlayerRespawnDelay, false);

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

void AUPGameModeBase::SpawnBotTimerElapsed()
{
	if (CVarSpawnBots.GetValueOnGameThread() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Bot spawning disabled via cvar 'CVarSpawnBots'."));
		return;
	}

	int32 NrOfAliveBots = 0;
	for (TActorIterator<AUPAICharacter> It(GetWorld()); It; ++It)
	{
		const AUPAICharacter* Bot = *It;

		const UUPAttributeComponent* AttributeComp = UUPAttributeComponent::GetAttributes(Bot);
		if (ensure(AttributeComp) && AttributeComp->IsAlive())
		{
			NrOfAliveBots++;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Found %i alive bots."), NrOfAliveBots);

	int32 MaxBotCount = 3;

	if (DifficultyCurve)
	{
		MaxBotCount = DifficultyCurve->GetFloatValue(GetWorld()->TimeSeconds);
	}

	if (NrOfAliveBots >= MaxBotCount)
	{
		UE_LOG(LogTemp, Log, TEXT("At maximum bot capacity. Skipping bot spawn."));
		return;
	}

	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, SpawnBotQuery, this, EEnvQueryRunMode::RandomBest25Pct, nullptr);
	if (ensure(QueryInstance))
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &AUPGameModeBase::OnBotSpawnQueryCompleted);
	}
}

void AUPGameModeBase::OnBotSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance,
	EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn bot EQS Query failed!"));
		return;
	}

	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();
	if (Locations.Num() > 0)
	{
		if (MonsterTable)
		{
			TArray<FMonsterInfoRow*> Rows;
			MonsterTable->GetAllRows("", Rows);

			const int32 RandomIndex = FMath::RandRange(0, Rows.Num() - 1);
			const FMonsterInfoRow* SelectedRow = Rows[RandomIndex];

			if (UAssetManager* Manager = UAssetManager::GetIfInitialized())
			{
				LogOnScreen(this, FString::Printf(TEXT("Loading monster '%s' ..."), *SelectedRow->MonsterId.PrimaryAssetName.ToString()), FColor::Green);

				// The basic idea of bundles is which part of the asset to load (e.g. in case of multiple soft references)
				const TArray<FName> Bundles;
				FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &AUPGameModeBase::OnMonsterLoaded, SelectedRow->MonsterId, Locations[0]);
				Manager->LoadPrimaryAsset(SelectedRow->MonsterId, Bundles, Delegate);
			}
		}
	}
}

void AUPGameModeBase::OnMonsterLoaded(FPrimaryAssetId LoadedId, FVector SpawnLocation)
{
	LogOnScreen(this, FString::Printf(TEXT("Finished loading '%s'."), *LoadedId.PrimaryAssetName.ToString()), FColor::Green);

	if (const UAssetManager* Manager = UAssetManager::GetIfInitialized())
	{
		if (UUPMonsterData* MonsterData = Cast<UUPMonsterData>(Manager->GetPrimaryAssetObject(LoadedId)))
		{
			if (AActor* NewBot = GetWorld()->SpawnActor<AActor>(MonsterData->MonsterClass, SpawnLocation, FRotator::ZeroRotator))
			{
				LogOnScreen(this, FString::Printf(TEXT("Spawned enemy: %s (%s)"), *GetNameSafe(NewBot), *GetNameSafe(MonsterData)));

				if (UUPActionComponent* ActionComp = Cast<UUPActionComponent>(NewBot->GetComponentByClass(UUPActionComponent::StaticClass())))
				{
					for (const TSubclassOf<UUPAction> ActionClass : MonsterData->Actions)
					{
						ActionComp->AddAction(NewBot, ActionClass);
					}
				}

				DrawDebugSphere(GetWorld(), SpawnLocation, 50.0f, 20, FColor::Blue, false, 60.0f);
			}	
		}
	}
}

void AUPGameModeBase::OnPowerupSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn bot EQS Query Failed!"));
		return;
	}

	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();

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
		int32 RandomClassIndex = FMath::RandRange(0, PowerupClasses.Num() - 1);
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
