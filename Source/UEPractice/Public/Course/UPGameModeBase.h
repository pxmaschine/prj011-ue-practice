// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UPGameModeBase.generated.h"

namespace EEnvQueryStatus
{
	enum Type : int;
}
class UEnvQuery;
class UCurveFloat;

UCLASS()
class UEPRACTICE_API AUPGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AUPGameModeBase();

public:
	virtual void StartPlay() override;

public:
	UFUNCTION(Exec)
	void KillAll();

protected:
	UFUNCTION()
	void SpawnBotTimerElapsed();

	UFUNCTION()
	void OnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TSubclassOf<AActor> MinionClass;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UEnvQuery* SpawnBotQuery;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UCurveFloat* DifficultyCurve;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float SpawnTimerInterval;

	FTimerHandle TimerHandle_SpawnBots;
};
