// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPGameViewportClient.h"

#include "SignificanceManager.h"
#include "Kismet/GameplayStatics.h"


void UUPGameViewportClient::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (USignificanceManager* SignificanceManager = USignificanceManager::Get(World))
	{
		// Iterate all playercontrollers, for hosting player this means all clients too
		// Helps keeping significance in sync between each client and server
		// ** Main desync issue right now could be WasRecentlyRendered which isn't going to be replicated on the host **
		ViewpointsArray.Empty(GetWorld()->GetNumPlayerControllers());
		for( FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			const APlayerController* PlayerController = Iterator->Get();

			FVector ViewLocation;
			FRotator ViewRotation;
			PlayerController->GetPlayerViewPoint(ViewLocation, ViewRotation);

			ViewpointsArray.Add(FTransform(ViewRotation, ViewLocation, FVector::OneVector));
		}

		SignificanceManager->Update(ViewpointsArray);
	}
}
