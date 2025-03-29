// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettingsBackedByCVars.h"
#include "UPDeveloperSettings.generated.h"


UCLASS(Config=EditorPerProjectUserSettings)
class UEPRACTICE_API UUPDeveloperSettings : public UDeveloperSettingsBackedByCVars
{
	GENERATED_BODY()

public:
	UPROPERTY(config, EditDefaultsOnly, Category = UEPractice, meta = (ConsoleVariable = "up.DisableBotSpawning"))
	bool bDisableSpawnBotsOverride = false;

	//~UDeveloperSettings interface
	virtual void PostInitProperties() override;

	virtual FName GetCategoryName() const override;
	//~End of UDeveloperSettings interface
};
