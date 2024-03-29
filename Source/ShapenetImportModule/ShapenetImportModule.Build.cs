// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class ShapenetImportModule : ModuleRules
{
    public ShapenetImportModule(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UnrealEd", "FBX", "Json", "JsonUtilities" });

        PrivateDependencyModuleNames.AddRange(new string[] { "SceneGenUnreal", "FBX", "Projects" });

        // Uncomment if you are using Slate UI
         PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

        PublicIncludePaths.AddRange(
            new string[]
            {
                "ShapenetImportModule/Public"
            });

        PrivateIncludePaths.AddRange(
            new string[]
            {
            "ShapenetImportModule/Private"
            });
    }
}
