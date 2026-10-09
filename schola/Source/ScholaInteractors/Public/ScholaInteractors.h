// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

/**
 * @brief Module class for ScholaInteractors plugin.
 * 
 * This module provides actuator and sensor interfaces and implementations
 * for Schola-based reinforcement learning agents in Unreal Engine.
 */
class FScholaInteractorsModule : public IModuleInterface
{
public:
    /**
     * @brief Called when the module is loaded during engine startup.
     */
    virtual void StartupModule() override;
    
    /**
     * @brief Called when the module is unloaded during engine shutdown.
     */
    virtual void ShutdownModule() override;
};
