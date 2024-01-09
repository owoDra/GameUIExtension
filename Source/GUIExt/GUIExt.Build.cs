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
                "Core",
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "InputCore",
                "UMG",
                "PreLoadScreen",
                "RenderCore",
                "ApplicationCore",
                "CommonUI",
                "CommonInput",
                "GameplayTags",
                "GameFeatures",
                "ControlFlows",
                "DeveloperSettings",
                "ModularGameplay",
            }
        );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "GFCore",
            }
        );
    }
}
