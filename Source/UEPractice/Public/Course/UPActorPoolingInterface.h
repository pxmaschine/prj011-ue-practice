// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UPActorPoolingInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UUPActorPoolingInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class UEPRACTICE_API IUPActorPoolingInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void PoolBeginPlay();

	UFUNCTION(BlueprintNativeEvent)
	void PoolEndPlay();
};
