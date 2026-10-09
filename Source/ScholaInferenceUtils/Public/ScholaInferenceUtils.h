// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

/**
 * @brief Module interface for the ScholaInferenceUtils plugin.
 * 
 * Provides utilities for AI inference including various stepper implementations
 * for managing agent-policy interactions.
 */
class FScholaInferenceUtilsModule : public IModuleInterface
{
public:
    /**
     * @brief Called when the module is loaded and initialized.
     * 
     * Performs module initialization tasks.
     */
    virtual void StartupModule() override;
    
    /**
     * @brief Called when the module is unloaded.
     * 
     * Performs module cleanup tasks.
     */
    virtual void ShutdownModule() override;
};