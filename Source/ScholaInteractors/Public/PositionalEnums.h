// Copyright (c) 2023 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * @brief Enum of frame of reference types for spatial observations and actions.
 * 
 * Defines how spatial information (position, rotation) is interpreted relative to
 * the world, another actor, or the agent itself.
 */
UENUM(BlueprintType)
enum EFrameOfReference : uint8
{
	/** Rotation and Position are both relative to a target Actor. Useful for agents that primarily navigate forward. */
	Egocentric,
	/** Position is relative to a target actor. */
	Relative,
	/** Position is based on world position. */
	World
};
