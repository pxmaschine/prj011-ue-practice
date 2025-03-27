// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameViewportClient.h"
#include "UPGameViewportClient.generated.h"

/**
 * 
 */
UCLASS()
class UEPRACTICE_API UUPGameViewportClient : public UGameViewportClient
{
	GENERATED_BODY()

protected:
	virtual void Tick(float DeltaTime) override;
};
