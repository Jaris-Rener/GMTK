// Copyright (c) 2024, Evelyn Schwab. All rights reserved.

using UnrealBuildTool;

public class CameraDynamicsEditor : ModuleRules
{
    public CameraDynamicsEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        bUseUnity = false;
        bUsePrecompiled = false;
        PrivatePCHHeaderFile = "Public/CameraDynamicsEditor.h";
        
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", "PropertyEditor",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "DeveloperSettings",
                "EditorStyle",
                "CameraDynamics", 
                "EditorScriptingUtilities", 
                "Blutility",
                "UMGEditor",
                "UMG",
                "ToolMenus",
                "Projects",
                "UnrealEd",
            }
        );
    }
}