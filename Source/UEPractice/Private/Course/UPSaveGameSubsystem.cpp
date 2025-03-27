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
	if (NewPlayer == nullptr || NewPlayer->IsPendingKillPending())
	{
		return false;
	}

	APlayerState* PS = NewPlayer->GetPlayerState<APlayerState>();
	if (PS == nullptr)
	{
		return false;
	}
	
	if (APawn* MyPawn = PS->GetPawn())
	{
		FPlayerSaveData* FoundData = CurrentSaveGame->GetPlayerData(PS);
		if (FoundData && FoundData->bResumeAtTransform)
		{		
			MyPawn->SetActorLocation(FoundData->Location);
			MyPawn->SetActorRotation(FoundData->Rotation);

			// PlayerState owner is a (Player)Controller
			AController* PC = Cast<AController>(PS->GetOwner());
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
	CurrentSaveGame->SavedActors.Empty();

	AGameStateBase* GS = GetWorld()->GetGameState();
	if (GS == nullptr)
	{
		// Warn about failure to save?
		return;
	}
	
	// Iterate all player states, we don't have proper ID to match yet (requires Steam or EOS)
	for (int32 i = 0; i < GS->PlayerArray.Num(); i++)
	{
		AUPPlayerState* PS = Cast<AUPPlayerState>(GS->PlayerArray[i]);
		if (PS)
		{
			PS->SavePlayerState(CurrentSaveGame);
			break; // single player only at this point
		}
	}

	// Iterate the entire world of actors
	for (FActorIterator It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
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

		CurrentSaveGame->SavedActors.Add(ActorData);
	}

	UGameplayStatics::SaveGameToSlot(CurrentSaveGame, CurrentSlotName, 0);

	OnSaveGameWritten.Broadcast(CurrentSaveGame);
}

void UUPSaveGameSubsystem::LoadSaveGame(FString InSlotName)
{
	// Update slot name first if specified, otherwise keeps default name
	SetSlotName(InSlotName);
	
	if (UGameplayStatics::DoesSaveGameExist(CurrentSlotName, 0))
	{
		CurrentSaveGame = Cast<UUPSaveGame>(UGameplayStatics::LoadGameFromSlot(CurrentSlotName, 0));
		if (CurrentSaveGame == nullptr)
		{
			UE_LOGFMT(LogGame, Warning, "Failed to load SaveGame Data.");
			return;
		}

		UE_LOGFMT(LogGame, Log, "Loaded SaveGame Data.");
	}
	else
	{
		CurrentSaveGame = Cast<UUPSaveGame>(UGameplayStatics::CreateSaveGameObject(UUPSaveGame::StaticClass()));

		UE_LOGFMT(LogGame, Log, "Created New SaveGame Data.");
	}
}

void UUPSaveGameSubsystem::ApplyLoadedSaveGame() const
{
	if (CurrentSaveGame->SavedActors.Num() == 0)
	{
		return;
	}

	// Iterate the entire world of Actors
	for (FActorIterator It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		// Only interested in our 'gameplay actors', skip actors that are being destroyed
		// Note: You might instead use a dedicated SavableObject interface for Actors you want to save instead of re-using GameplayInterface
		if (Actor->IsPendingKillPending() || !Actor->Implements<UUPGameplayInterface>())
		{
			continue;
		}

		for (const FActorSaveData& ActorData : CurrentSaveGame->SavedActors)
		{
			if (ActorData.ActorName == Actor->GetFName())
			{
				Actor->SetActorTransform(ActorData.Transform);

				FMemoryReader MemReader(ActorData.ByteData);

				FObjectAndNameAsStringProxyArchive Ar(MemReader, true);
				// Find only variables with UPROPERTY(SaveGame)
				Ar.ArIsSaveGame = true;
				// Converts binary array into Actor's PROPERTIES 
				Actor->Serialize(Ar);

				IUPGameplayInterface::Execute_OnActorLoaded(Actor);

				break;
			}
		}
	}

	OnSaveGameLoaded.Broadcast(CurrentSaveGame);
}
