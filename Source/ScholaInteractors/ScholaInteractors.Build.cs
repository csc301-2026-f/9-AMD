// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

using UnrealBuildTool;

/// <summary>
/// Build configuration for the ScholaInteractors module.
/// 
/// This module provides actuator and sensor interfaces and implementations
/// for Schola reinforcement learning agents. It depends on the core Schola module
/// and provides components that can be attached to actors to enable agent interaction
/// with the Unreal Engine environment.
/// </summary>
public class ScholaInteractors : ModuleRules
{
    /// <summary>
    /// Constructor that configures module dependencies and include paths.
    /// </summary>
    /// <param name="Target">The target being built</param>
    public ScholaInteractors(ReadOnlyTargetRules Target) : base(Target)
    {
        // Use explicit or shared precompiled headers
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        if (System.Environment.GetEnvironmentVariable("SCHOLA_MEASURE_CPP_COVERAGE") == "1")
        {
            OptimizeCode = CodeOptimization.Never;
        }

        // Public include paths - none needed beyond defaults
        PublicIncludePaths.AddRange(new string[] { });

        // Private include paths for implementation files
        PrivateIncludePaths.AddRange(new string[] { "ScholaInteractors/Private"});

        // Public module dependencies (exposed to consumers of this module)
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",      // Core Unreal Engine functionality
            "Schola"     // Schola RL framework (Spaces, Points, etc.)
        });

        // Private include path module dependencies
        PrivateIncludePathModuleNames.AddRange(new string[] { });
        
        // Private module dependencies (only used internally)
        PrivateDependencyModuleNames.AddRange(new string[] {
            "CoreUObject", // UObject system
            "Engine",      // Unreal Engine core (Actors, Components, etc.)
            "Projects",    // Project management functionality
        });

        // Modules that should be dynamically loaded at runtime
        DynamicallyLoadedModuleNames.AddRange(new string[] { });
    }
}
