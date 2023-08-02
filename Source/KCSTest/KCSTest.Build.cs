// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class KCSTest : ModuleRules
{
	public KCSTest(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
	        "Core",
	        "CoreUObject",
	        "Engine",
	        "InputCore",
	        "HeadMountedDisplay",
	        "NavigationSystem",
	        "AIModule",
	        "Niagara",
	        "EnhancedInput",
	        "DeveloperSettings",
	        "GameplayTasks",
	        "UMG"
        });
        
        // Plugins
        PublicDependencyModuleNames.AddRange( new string[]
        {
	        "GameplayTags"
        });
    }
}
