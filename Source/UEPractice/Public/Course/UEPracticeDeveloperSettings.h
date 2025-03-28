// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettingsBackedByCVars.h"
#include "UEPracticeDeveloperSettings.generated.h"


namespace DevelopmentOnly
{
	static bool bSpawnBotsOverride = false;
	static FAutoConsoleVariableRef CVarSpawnBotsOverride(
	TEXT("up.SpawnBotsOverride"),
	bSpawnBotsOverride,
	TEXT("Enable spawning of bots via timer."));

}

UCLASS(Config=EditorPerProjectUserSettings)
class UEPRACTICE_API UUEPracticeDeveloperSettings : public UDeveloperSettingsBackedByCVars
{
	GENERATED_BODY()

public:
	UPROPERTY(config, EditDefaultsOnly, Category = ActionRoguelike, meta = (ConsoleVariable = "up.SpawnBotsOverride"))
	bool bSpawnBotsOverride = false;

	//~UDeveloperSettings interface
	virtual FName GetCategoryName() const override;
	//~End of UDeveloperSettings interface
};
