// Copyright (C) 2024 owoDra

using UnrealBuildTool;

public class GUIExt : ModuleRules
{
	public GUIExt(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[]
            {
                ModuleDirectory,
                ModuleDirectory + "/GUIExt",
            }
        );


        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", "CoreUObject", "Engine",

                "Slate", "SlateCore", "UMG",

                "CommonUI", "CommonInput",
                
                "GameplayTags",

                "GFCore",
            }
        );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "DeveloperSettings",

                "InputCore", "EnhancedInput",

                "GameFeatures", "ModularGameplay",
            }
        );
    }
}
