// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "TrainingUtils/ArgBuilder.h"
#include "TrainingSettings.generated.h"

/**
 * @brief Abstract class for any training settings
 */
USTRUCT(BlueprintType)
struct SCHOLATRAINING_API FTrainingSettings
{
	GENERATED_BODY()

public:
	/**
	 * @brief Generate the training arguments for the script using the ArgBuilder.
	 * @param[in,out] ArgBuilder The builder to use to generate the arguments.
	 * @details Populates the ArgBuilder with training-specific command-line arguments.
	 */
	virtual void GenerateTrainingArgs(FScriptArgBuilder& ArgBuilder) const;

	virtual ~FTrainingSettings() {};
};

