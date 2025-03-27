// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPGameViewportClient.h"

#include "SignificanceManager.h"
#include "Kismet/GameplayStatics.h"


void UUPGameViewportClient::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (USignificanceManager* SignificanceManager = USignificanceManager::Get(World))
	{
		// Update once per frame, using only Player 0
		if (const APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0))
		{
			FVector ViewLocation;
			FRotator ViewRotation;
			PC->GetPlayerViewPoint(ViewLocation, ViewRotation);

			// Viewpoints
			TArray<FTransform> TransformArray;
			TransformArray.Emplace(ViewRotation, ViewLocation, FVector::OneVector);

			SignificanceManager->Update(TArrayView<FTransform>(TransformArray));
		}
	}
}
