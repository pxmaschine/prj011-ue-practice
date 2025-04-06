// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPSaveGameSubsystem.h"
#include "Course/UPGameplayInterface.h"
#include "Course/UPPlayerState.h"
#include "Course/UPSaveGame.h"
#include "Course/UPSaveGameSettings.h"
#include "UEPractice/UEPractice.h"

#include "EngineUtils.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"


void UUPSaveGameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UUPSaveGameSettings* SGSettings = GetDefault<UUPSaveGameSettings>();
	// Access defaults from DefaultGame.ini
	CurrentSlotName = SGSettings->SaveSlotName;

	// Make sure it's loaded into memory .Get() only resolves if already loaded previously elsewhere in code
	UDataTable* DummyTable = SGSettings->DummyTablePath.LoadSynchronous();
	//DummyTable->GetAllRows() // We don't need this table for anything, just an content reference example
}

void UUPSaveGameSubsystem::HandleStartingNewPlayer(AController* NewPlayer)
{
	AUPPlayerState* PS = NewPlayer->GetPlayerState<AUPPlayerState>();
	if (ensure(PS))
	{
		PS->LoadPlayerState(CurrentSaveGame);
	}
}

bool UUPSaveGameSubsystem::OverrideSpawnTransform(AController* NewPlayer)
{
	check(NewPlayer);

	APlayerState* PS = NewPlayer->GetPlayerState<APlayerState>();
	check(PS);

	if (APawn* MyPawn = PS->GetPawn())
	{
		FPlayerSaveData* FoundData = CurrentSaveGame->GetPlayerData(PS);
		if (FoundData && FoundData->bResumeAtTransform)
		{		
			MyPawn->SetActorLocation(FoundData->Location);
			MyPawn->SetActorRotation(FoundData->Rotation);

			// PlayerState owner is a (Player)Controller
			AController* PC = CastChecked<AController>(PS->GetOwner());
			// Set control rotation to change camera direction, setting Pawn rotation is not enough
			PC->SetControlRotation(FoundData->Rotation);
			
			return true;
		}
	}

	return false;
}

void UUPSaveGameSubsystem::SetSlotName(FString NewSlotName)
{
	// Ignore empty name
	if (NewSlotName.Len() == 0)
	{
		return;
	}
	
	CurrentSlotName = NewSlotName;
}

void UUPSaveGameSubsystem::WriteSaveGame()
{
	// Clear arrays, may contain data from previously loaded SaveGame
	CurrentSaveGame->SavedPlayers.Empty();
	CurrentSaveGame->SavedActorMap.Empty();

	AGameStateBase* GS = GetWorld()->GetGameState();
	check(GS);
	
	// Iterate all player states, we don't have proper ID to match yet (requires Steam or EOS)
	for (int32 i = 0; i < GS->PlayerArray.Num(); i++)
	{
		AUPPlayerState* PS = CastChecked<AUPPlayerState>(GS->PlayerArray[i]);
		if (PS)
		{
			PS->SavePlayerState(CurrentSaveGame);
			break; // only single player supported at this point
		}
	}

	// Iterate the entire world of actors
	for (AActor* Actor : TActorRange<AActor>(GetWorld()))
	{
		// Only interested in our 'gameplay actors', skip actors that are being destroyed
		// Note: You might instead use a dedicated SavableObject interface for Actors you want to save instead of re-using GameplayInterface
		if (Actor->IsPendingKillPending() || !Actor->Implements<UUPGameplayInterface>())
		{
			continue;
		}

		FActorSaveData ActorData;
		ActorData.ActorName = Actor->GetFName();
		ActorData.Transform = Actor->GetActorTransform();
		
		// Pass the array to fill with data from Actor
		FMemoryWriter MemWriter(ActorData.ByteData);

		FObjectAndNameAsStringProxyArchive Ar(MemWriter, true);
		// Find only variables with UPROPERTY(SaveGame)
		Ar.ArIsSaveGame = true;
		// Converts Actor's SaveGame UPROPERTIES into binary array
		Actor->Serialize(Ar);

		CurrentSaveGame->SavedActorMap.Add(Actor->GetFName(), ActorData);
	}

	UGameplayStatics::SaveGameToSlot(CurrentSaveGame, CurrentSlotName, 0);

	OnSaveGameWritten.Broadcast(CurrentSaveGame);
}

void UUPSaveGameSubsystem::LoadSaveGame(FString InSlotName)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(LoadSaveGame);

	// Update slot name first if specified, otherwise keeps default name
	SetSlotName(InSlotName);
	
	if (UGameplayStatics::DoesSaveGameExist(CurrentSlotName, 0))
	{
		CurrentSaveGame = CastChecked<UUPSaveGame>(UGameplayStatics::LoadGameFromSlot(CurrentSlotName, 0));
		if (CurrentSaveGame == nullptr)
		{
			UE_LOGFMT(LogGame, Warning, "Failed to load SaveGame Data.");
			return;
		}

		UE_LOGFMT(LogGame, Log, "Loaded SaveGame Data.");
	}
	else
	{
		CurrentSaveGame = CastChecked<UUPSaveGame>(UGameplayStatics::CreateSaveGameObject(UUPSaveGame::StaticClass()));

		UE_LOGFMT(LogGame, Log, "Created New SaveGame Data.");
	}
}

void UUPSaveGameSubsystem::ApplyLoadedSaveGame() const
{
	if (CurrentSaveGame->SavedActorMap.Num() == 0)
	{
		return;
	}

	// Iterate the entire world of Actors
	for (AActor* Actor : TActorRange<AActor>(GetWorld()))
	{
		// Only interested in our 'gameplay actors', skip actors that are being destroyed
		// Note: You might instead use a dedicated SavableObject interface for Actors you want to save instead of re-using GameplayInterface
		if (Actor->IsPendingKillPending() || !Actor->Implements<UUPGameplayInterface>())
		{
			continue;
		}

		if (FActorSaveData* FoundData = CurrentSaveGame->SavedActorMap.Find(Actor->GetFName()))
		{
			Actor->SetActorTransform(FoundData->Transform);

			FMemoryReader MemReader(FoundData->ByteData);

			FObjectAndNameAsStringProxyArchive Ar(MemReader, true);
			Ar.ArIsSaveGame = true;
			// Convert binary array back into actor's variables
			Actor->Serialize(Ar);

			IUPGameplayInterface::Execute_OnActorLoaded(Actor);
		}
	}

	OnSaveGameLoaded.Broadcast(CurrentSaveGame);
}
