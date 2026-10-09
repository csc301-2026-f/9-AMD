// Copyright (c) 2026 Advanced Micro Devices, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ScholaStateTree : ModuleRules
{
    public ScholaStateTree(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        if (System.Environment.GetEnvironmentVariable("SCHOLA_MEASURE_CPP_COVERAGE") == "1")
        {
            OptimizeCode = CodeOptimization.Never;
        }

        PublicIncludePaths.AddRange(new string[] { });
        PrivateIncludePaths.AddRange(new string[] { "ScholaStateTree/Private" });

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "AIModule",
            "GameplayTags",
            "StateTreeModule",
            "GameplayStateTreeModule",
            "Schola",
            "ScholaTraining"
        });

        PrivateDependencyModuleNames.AddRange(new string[] { });
    }
}
