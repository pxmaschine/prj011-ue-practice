// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPPlayerState.h"

#include <Course/UPSaveGame.h>

#include "Net/UnrealNetwork.h"


void AUPPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
 {
 	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
 
 	DOREPLIFETIME(AUPPlayerState, Credits);
 }

int32 AUPPlayerState::GetCredits() const
{
	return Credits;
}

void AUPPlayerState::AddCredits(int32 Delta)
{
	// Avoid user-error of adding a negative amount or zero
	if (!ensure(Delta > 0.0f))
	{
		return;
	}

	Credits += Delta;

	OnCreditsChanged.Broadcast(this, Credits, Delta);
}

bool AUPPlayerState::RemoveCredits(int32 Delta)
{
	// Avoid user-error of adding a subtracting negative amount or zero
	if (!ensure(Delta > 0.0f))
	{
		return false;
	}

	if (Credits < Delta)
	{
		// Not enough credits available
		return false;
	}

	Credits -= Delta;

	OnCreditsChanged.Broadcast(this, Credits, -Delta);

	return true;
}

void AUPPlayerState::SavePlayerState_Implementation(UUPSaveGame* SaveGame)
{
	if (SaveGame)
	{
		SaveGame->Credits = Credits;
	}
}

void AUPPlayerState::LoadPlayerState_Implementation(UUPSaveGame* SaveGame)
{
	if (SaveGame)
	{
		Credits = SaveGame->Credits;
	}
}

void AUPPlayerState::OnRep_Credits(int32 OldCredits)
{
	OnCreditsChanged.Broadcast(this, Credits, Credits - OldCredits);
}
