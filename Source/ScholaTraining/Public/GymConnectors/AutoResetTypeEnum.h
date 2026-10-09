// Copyright (c) 2023-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "AutoResetTypeEnum.generated.h"

/**
 * @brief Enumeration defining automatic environment reset behavior for gym connectors.
 * @details Controls when and how environments are automatically reset after episode termination.
 */
UENUM()
enum class EAutoResetType
{
	/** Only reset the environment when we receive an explicit reset message. */
	Disabled UMETA(DisplayName = "Disabled"),
	/** Not Supported. Reset the environment with the first step (so we discard an action). */
	NextStep UMETA(DisplayName = "NextStep"),
	/** Reset the environment mid-step and put the last observation in the info dictionary. */
	SameStep UMETA(DisplayName = "SameStep"),
};
