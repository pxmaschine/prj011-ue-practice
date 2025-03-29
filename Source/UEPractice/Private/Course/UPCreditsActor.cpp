// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPCreditsActor.h"
#include "Course/UPPlayerState.h"

AUPCreditsActor::AUPCreditsActor()
{
	CreditsAmount = 80;

	bCanAutoPickup = true;
}

void AUPCreditsActor::Interact_Implementation(APawn* InstigatorPawn)
{
	Super::Interact_Implementation(InstigatorPawn);

	if (AUPPlayerState* PS = InstigatorPawn->GetPlayerState<AUPPlayerState>())
	{
		PS->AddCredits(CreditsAmount);
		HideAndCooldownPickUp();
	}
}
