// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

/**
 * @brief Unreal module implementation for Schola protobuf / gRPC communication.
 */
class FScholaProtobufModule : public IModuleInterface
{
public:
    /** Loads protobuf-related subsystems and communication backends. */
    virtual void StartupModule() override;
    /** Shuts down communication backends and releases module resources. */
    virtual void ShutdownModule() override;
};
