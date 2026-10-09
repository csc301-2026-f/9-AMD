// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

/**
 * @brief Schola Training module for Unreal Engine reinforcement learning integration.
 * @details This module provides functionality for training RL agents within Unreal Engine environments,
 * including gym connectors, environment interfaces, and training utilities.
 */
class FScholaTrainingModule : public IModuleInterface
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