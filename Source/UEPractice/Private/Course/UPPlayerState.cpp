// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPPlayerState.h"

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
