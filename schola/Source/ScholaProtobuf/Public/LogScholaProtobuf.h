// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Logging/StructuredLog.h"

/**
 * @file LogScholaProtobuf.h
 * @brief Declares log category for the ScholaProtobuf module.
 * 
 * This file defines the logging category used for the ScholaProtobuf module,
 * which handles protobuf serialization and gRPC communication.
 */

/**
 * @brief Log category for the ScholaProtobuf module.
 * 
 * Use this category for logging related to protobuf serialization,
 * gRPC communication, and message handling.
 */
SCHOLAPROTOBUF_API DECLARE_LOG_CATEGORY_EXTERN(LogScholaProtobuf, Log, All);

