// Copyright (c) 2024 Advanced Micro Devices, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

/**
 * @class FScholaModule
 * @brief Main module class for the Schola plugin.
 * 
 * This class implements the IModuleInterface and handles the initialization
 * and shutdown of the Schola plugin module for Unreal Engine.
 */
class FScholaModule : public IModuleInterface
{
	/**
	 * @brief Called when the module is loaded during engine startup.
	 * 
	 * This method is invoked automatically by the module manager when the
	 * module is loaded. It handles initialization tasks such as registering
	 * settings and setting up module resources.
	 */
	virtual void StartupModule() override;
	
	/**
	 * @brief Called when the module is unloaded during engine shutdown.
	 * 
	 * This method is invoked automatically by the module manager when the
	 * module is unloaded. It handles cleanup tasks such as unregistering
	 * settings and releasing module resources.
	 */
	virtual void ShutdownModule() override;
};
