// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "TrainingUtils/ArgBuilder.h"
#include "ImitationSettings.generated.h"

/**
 * @brief Abstract base class for any imitation learning framework settings.
 * @details Provides a common interface for generating command-line arguments
 * across different imitation learning frameworks (e.g., Minari, custom tools).
 */
USTRUCT(BlueprintType)
struct SCHOLAIMITATION_API FImitationSettings
{
	GENERATED_BODY()

public:
	/**
	 * @brief Generate the imitation learning arguments for the script using the ArgBuilder.
	 * @param[in,out] ArgBuilder The builder to use to generate the arguments.
	 * @details Populates the ArgBuilder with framework-specific command-line arguments.
	 */
	virtual void GenerateImitationArgs(FScriptArgBuilder& ArgBuilder) const;

	virtual ~FImitationSettings() {};
};
