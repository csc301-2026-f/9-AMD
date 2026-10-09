// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.
using UnrealBuildTool;

public class ScholaInferenceUtils : ModuleRules
{
    public ScholaInferenceUtils(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        if (System.Environment.GetEnvironmentVariable("SCHOLA_MEASURE_CPP_COVERAGE") == "1")
        {
            OptimizeCode = CodeOptimization.Never;
        }

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "Schola"
        });

        PrivateDependencyModuleNames.AddRange(new string[] { });
    }
}