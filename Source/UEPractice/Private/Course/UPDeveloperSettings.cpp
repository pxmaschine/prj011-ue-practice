// Fill out your copyright notice in the Description page of Project Settings.


#include "Course/UPDeveloperSettings.h"

#include "Misc/ConfigUtilities.h"


namespace DevelopmentOnly
{
	static bool bDisableSpawnBotsOverride = false;
	static FAutoConsoleVariableRef CVarSpawnBotsOverride(
	TEXT("up.DisableBotSpawning"),
	bDisableSpawnBotsOverride,
	TEXT("Disable spawning of bots for development only."));
}


void UUPDeveloperSettings::PostInitProperties()
{
#if WITH_EDITOR
	if (IsTemplate())
	{
		// We want the .ini file to have precedence over the CVar constructor, so we apply the ini to the CVar before following the regular UDeveloperSettingsBackedByCVars flow
		UE::ConfigUtilities::ApplyCVarSettingsFromIni(TEXT("/Script/UEPractice.UPDeveloperSettings"), *GEditorPerProjectIni, ECVF_SetByProjectSetting);
	}
#endif // WITH_EDITOR

	Super::PostInitProperties();
}

FName UUPDeveloperSettings::GetCategoryName() const
{
	return FApp::GetProjectName();
}

bool UUPDeveloperSettings::DisableSpawnBotsOverride()
{
	return DevelopmentOnly::bDisableSpawnBotsOverride;
}
