// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

/**
 * @brief Unreal Engine module for Schola Imitation Learning functionality.
 * @details This module provides the infrastructure for imitation learning in Schola,
 * enabling environments to collect expert demonstrations and learn from them.
 */
class FScholaImitationModule : public IModuleInterface
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