// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

/**
 * @brief Module interface for the ScholaNNE plugin
 * 
 * This class provides the entry point for the ScholaNNE module which integrates
 * Unreal Engine's Neural Network Engine (NNE) with the Schola reinforcement learning framework.
 */
class FScholaNNEModule : public IModuleInterface
{
public:
	/**
	 * @brief Called when the module is loaded
	 */
	virtual void StartupModule() override;
	
	/**
	 * @brief Called when the module is unloaded
	 */
	virtual void ShutdownModule() override;
};