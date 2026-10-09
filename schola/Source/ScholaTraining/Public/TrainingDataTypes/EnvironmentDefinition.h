// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Common/InteractionDefinition.h"
#include "EnvironmentDefinition.generated.h"

/**
 * @brief Struct containing the properties that define an environment.
 * @details Defines all agents within an environment and their observation/action spaces.
 * This definition is shared between the GymConnector and the Environment objects to
 * ensure consistent communication about agent capabilities.
 */
USTRUCT(BlueprintType)
struct SCHOLATRAINING_API FEnvironmentDefinition
{
	GENERATED_BODY()

	/**
	 * @brief Map from agent name to agent interaction definitions.
	 * 
	 * Each entry defines an agent's observation and action spaces, allowing
	 * for heterogeneous multi-agent environments where different agents may
	 * have different capabilities.
	 */
	UPROPERTY(BlueprintReadWrite, Category = "Schola|Training")
	TMap<FString, FInteractionDefinition> AgentDefinitions;

};
