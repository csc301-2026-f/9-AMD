// Copyright (c) 2026 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "StructUtils/InstancedStruct.h"
#include "Spaces/Space.h"
#include "Points/Point.h"

class UEnhancedPlayerInput;

/**
 * @brief Utility functions for converting Enhanced Input to Schola spaces/points.
 * @details Static functions that can be used by imitation player controllers and tests.
 */
class SCHOLAIMITATION_API FEnhancedInputUtils
{
public:
	/**
	 * @brief Build action space from an Input Mapping Context.
	 * @details Iterates through all actions in the IMC and creates the appropriate
	 * space types based on each action's value type:
	 * - Boolean -> FMultiBinarySpace (dims: 1)
	 * - Axis1D -> FBoxSpace (low: -1, high: 1)
	 * - Axis2D -> FBoxSpace (low: [-1,-1], high: [1,1])
	 * - Axis3D -> FBoxSpace (low: [-1,-1,-1], high: [1,1,1])
	 *
	 * @param[in] MappingContext The IMC to analyze
	 * @param[out] OutActionSpace The action space as TInstancedStruct<FSpace> (FDictSpace)
	 * @return True if successful, false if MappingContext is null
	 */
	static bool BuildActionSpaceFromIMC(UInputMappingContext* MappingContext, TInstancedStruct<FSpace>& OutActionSpace);

	/**
	 * @brief Poll current action values from an Input Mapping Context.
	 * @details Reads the current input state for all actions in the IMC and converts
	 * them to the appropriate point types:
	 * - Boolean -> FMultiBinaryPoint (0 or 1)
	 * - Axis1D -> FBoxPoint (single float)
	 * - Axis2D -> FBoxPoint (X, Y)
	 * - Axis3D -> FBoxPoint (X, Y, Z)
	 *
	 * @param[in] MappingContext The IMC to poll
	 * @param[in] EnhancedInput The player input to read values from
	 * @param[out] OutActions The actions as TInstancedStruct<FPoint> (FDictPoint)
	 * @return True if successful, false if MappingContext or EnhancedInput is null
	 */
	static bool PollActionsFromIMC(UInputMappingContext* MappingContext, UEnhancedPlayerInput* EnhancedInput, TInstancedStruct<FPoint>& OutActions);
};
