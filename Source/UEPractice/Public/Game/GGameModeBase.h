// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GGameModeBase.generated.h"

class UEnvQuery;
struct FEnvQueryResult;

UCLASS()
class UEPRACTICE_API AGGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGGameModeBase();

public:
	virtual void StartPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	auto GetActiveEnemies() const -> const TArray<AActor*>& { return ActiveEnemies; }

protected:
	void StartSpawningEnemies();

	void SpawnEnemyTimerElapsed();

	void OnEnemySpawnQueryCompleted(TSharedPtr<FEnvQueryResult> Result);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	TSubclassOf<AActor> EnemyClass;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float SpawnTimerInterval;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	int32 MaxEnemyCount;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UEnvQuery* SpawnEnemyQuery;

	FTimerHandle TimerHandle_SpawnEnemy;

	TArray<AActor*> ActiveEnemies;
};
