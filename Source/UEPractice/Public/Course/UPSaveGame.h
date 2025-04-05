// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "UPSaveGame.generated.h"

class APlayerState;

USTRUCT()
struct FActorSaveData
{
	GENERATED_BODY()
	
public:
	// Identifier for which Actor this belongs to
	UPROPERTY()
	FName ActorName;

	// For movable Actors, keep location, rotation, scale.
	UPROPERTY()
	FTransform Transform;

	/* Contains all 'SaveGame' marked variables of the Actor */
	UPROPERTY()
	TArray<uint8> ByteData;
};

USTRUCT()
struct FPlayerSaveData
{
	GENERATED_BODY()

	FPlayerSaveData()
	{
		Credits = 0;
		PersonalRecordTime = 0.0f;
		Location = FVector::ZeroVector;
		Rotation = FRotator::ZeroRotator;
		bResumeAtTransform = true;
	}

public:
	/* Player Id defined by the online sub system. */ 
	UPROPERTY()
	FString PlayerID;

	UPROPERTY()
    int32 Credits;

	/* Longest survival time */
	UPROPERTY()
    float PersonalRecordTime;

	/* Location if player was alive during save */
	UPROPERTY()
	FVector Location;

	/* Orientation if player was alive during save */ 
	UPROPERTY()
	FRotator Rotation;

	/* We don't always want to restore location, and may just resume player at specific respawn point in world. */
	UPROPERTY()
	bool bResumeAtTransform;
};

UCLASS()
class UEPRACTICE_API UUPSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	FPlayerSaveData* GetPlayerData(APlayerState* PlayerState);

public:
	UPROPERTY()
	TArray<FPlayerSaveData> SavedPlayers;

	/* Actors stored from a level (currently does not support a specific level and just assumes the demo map) */
	UPROPERTY()
	TMap<FName, FActorSaveData> SavedActorMap;
};
