// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UEPractice : ModuleRules
{
	public UEPractice(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput", 
			"GameplayTags", 
			"OnlineSubsystem", 
			"SignificanceManager", 
			"Niagara",
			"CoreOnline", 
			"StructUtils"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"MoviePlayer" , // loading screen
			"RenderCore" // for PSO caching code access
		});
	}
}
