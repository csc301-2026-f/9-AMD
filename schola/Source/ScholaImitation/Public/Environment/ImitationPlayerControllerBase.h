// Copyright (c) 2026 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "StructUtils/InstancedStruct.h"
#include "Spaces/Space.h"
#include "Points/Point.h"
#include "Common/InstancedStructUtils.h"
#include "ImitationPlayerControllerBase.generated.h"

/**
 * @brief Base class for imitation learning player controllers.
 * @details Provides common functionality for capturing Enhanced Input actions and converting
 * them to the Schola action space format. Both single-agent and multi-agent imitation
 * controllers inherit from this class.
 *
 * This class is not meant to be used directly - use AImitationPlayerController for
 * single-agent scenarios or AMultiAgentImitationPlayerController for multi-agent scenarios.
 */
UCLASS(Abstract)
class SCHOLAIMITATION_API AImitationPlayerControllerBase : public APlayerController
{
	GENERATED_BODY()

public:
	AImitationPlayerControllerBase();

	// ========== Enhanced Input Helpers ==========

	/**
	 * @brief Get the Enhanced Player Input subsystem for this controller.
	 * @return The UEnhancedPlayerInput for this controller, or nullptr if not available
	 */
	class UEnhancedPlayerInput* GetEnhancedPlayerInput() const;

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
	 * @param[out] OutActionSpace The action space as FDictSpace
	 * @return True if successful
	 */
	UFUNCTION(BlueprintCallable, Category = "Schola|Imitation")
	bool BuildActionSpaceFromIMC(UInputMappingContext* MappingContext, FInstancedStruct& OutActionSpace);

	/**
	 * @brief Type-safe wrapper for BuildActionSpaceFromIMC.
	 * @param[in] MappingContext The IMC to analyze
	 * @param[out] OutActionSpace The action space as TInstancedStruct<FSpace>
	 * @return True if successful
	 */
	bool BuildActionSpaceFromIMC(UInputMappingContext* MappingContext, TInstancedStruct<FSpace>& OutActionSpace)
	{
		return BuildActionSpaceFromIMC(MappingContext, ToUntypedInstancedStruct(OutActionSpace));
	}

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
	 * @param[out] OutActions The actions as FDictPoint
	 * @return True if successful
	 */
	UFUNCTION(BlueprintCallable, Category = "Schola|Imitation")
	bool PollActionsFromIMC(UInputMappingContext* MappingContext, FInstancedStruct& OutActions);

	/**
	 * @brief Type-safe wrapper for PollActionsFromIMC.
	 * @param[in] MappingContext The IMC to poll
	 * @param[out] OutActions The actions as TInstancedStruct<FPoint>
	 * @return True if successful
	 */
	bool PollActionsFromIMC(UInputMappingContext* MappingContext, TInstancedStruct<FPoint>& OutActions)
	{
		return PollActionsFromIMC(MappingContext, ToUntypedInstancedStruct(OutActions));
	}
};
