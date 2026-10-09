// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TrainingDataTypes/EnvironmentDefinition.h"
#include "TrainingDefinition.generated.h"


/**
 * @brief Struct containing the properties that define a training session.
 * @details This structure defines all environments and agents involved in a training session,
 * including their observation and action spaces. It serves as the configuration blueprint
 * for setting up distributed training across multiple environments and agents.
 */
USTRUCT(BlueprintType)
struct SCHOLATRAINING_API FTrainingDefinition
{
	GENERATED_BODY()
	
	/**
	 * @brief Array of environment definitions for this training session.
	 * 
	 * Each environment definition contains the agent definitions (observation and action spaces)
	 * for all agents in that environment. This allows for heterogeneous multi-agent setups
	 * across multiple parallel environments.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reinforcement Learning")
	TArray<FEnvironmentDefinition> EnvironmentDefinitions;
	
};