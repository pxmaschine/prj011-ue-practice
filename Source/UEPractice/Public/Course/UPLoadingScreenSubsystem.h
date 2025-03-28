// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UPLoadingScreenSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class UEPRACTICE_API UUPLoadingScreenSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

protected:
	void BeginLoadingScreen(const FString& MapName);

	void EndLoadingScreen(UWorld* InLoadedWorld);
};
