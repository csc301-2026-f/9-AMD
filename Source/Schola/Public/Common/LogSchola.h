// Copyright (c) 2023 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Logging/StructuredLog.h"

/**
 * @file LogSchola.h
 * @brief Declares log categories for the Schola plugin.
 * 
 * This file defines the logging categories used throughout the Schola plugin
 * for debugging and information output.
 */

/**
 * @brief Main log category for the Schola plugin.
 * 
 * Use this category for general Schola-related logging.
 */
SCHOLA_API DECLARE_LOG_CATEGORY_EXTERN(LogSchola, Log, All);

/**
 * @brief Log category for Schola communication and networking.
 * 
 * Use this category for logging related to agent communication,
 * network messages, and inter-process communication.
 */
SCHOLA_API DECLARE_LOG_CATEGORY_EXTERN(LogScholaCommunicator, Log, All);