// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Schola : ModuleRules
{
    public Schola(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        if (System.Environment.GetEnvironmentVariable("SCHOLA_MEASURE_CPP_COVERAGE") == "1")
        {
            OptimizeCode = CodeOptimization.Never;
        }

        // Suppress Clang warning for USTRUCTs with virtual functions (FPoint and FSpace hierarchies)
        // on UE 5.5 Linux. This warning is triggered by Unreal's TCppStructOps::Destruct calling
        // explicit destructor on non-final types with virtual functions.
        // The code is correct (virtual destructors exist), but Clang is strict about non-final types.
        // Note: UE 5.6 does not have this issue.
        if (Target.Platform == UnrealTargetPlatform.Linux && Target.Version.MajorVersion == 5 && Target.Version.MinorVersion == 5)
        {
            bWarningsAsErrors = false;
        }

        PublicIncludePaths.AddRange(new string[] { });

        PrivateIncludePaths.AddRange(new string[] { "Schola/Private"});

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "InputCore",
            "AIModule",
            "Json", 
            "JsonUtilities",
        });

        PrivateIncludePathModuleNames.AddRange(new string[] { });
        PrivateDependencyModuleNames.AddRange(new string[] {
            "CoreUObject",
            "Engine",
            "Slate",
            "SlateCore",
            "Projects"
        });
        
        // BlueprintGraph is only needed for the editor so we can raise BlueprintErrors
        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.Add("BlueprintGraph");
        }
        
        DynamicallyLoadedModuleNames.AddRange(new string[] { });
    }
}
