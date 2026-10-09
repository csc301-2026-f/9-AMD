// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Containers/SortedMap.h"
#include "StructUtils/InstancedStruct.h"
#include "Points/Point.h"
#include "EnvironmentUpdate.generated.h"

/**
 * @brief A struct representing an environment step update with agent actions.
 * @details Contains the actions to be executed by each agent in the environment
 * during a single simulation step.
 */
USTRUCT(BlueprintType)
struct SCHOLATRAINING_API FEnvStep
{
	GENERATED_BODY()
	
	/**
	 * @brief Map from agent ID to the action that agent should execute.
	 * 
	 * Each entry contains an action point that must conform to the agent's
	 * action space definition.
	 */
	UPROPERTY()
	TMap<FString, TInstancedStruct<FPoint>> Actions;

};

/**
 * @brief A struct representing an environment reset configuration.
 * @details Contains optional parameters for resetting an environment, including
 * random seed and custom configuration options.
 */
USTRUCT(BlueprintType)
struct SCHOLATRAINING_API FEnvReset
{
	GENERATED_BODY()

	/**
	 * @brief Custom configuration options for the reset.
	 * 
	 * These key-value pairs are environment-specific and can be used to
	 * configure the initial state or behavior of the environment.
	 */
	UPROPERTY(EditAnywhere, Category = "Environment Reset Options")
	TMap<FString, FString> Options;

	/**
	 * @brief Random seed for environment initialization.
	 * 
	 * Used to ensure reproducible resets when bHasSeed is true.
	 */
	UPROPERTY(EditAnywhere, Category = "Environment Reset Options")
	int					   Seed = 0;

	/**
	 * @brief Flag indicating whether a seed was provided.
	 * 
	 * When true, the Seed field should be used for environment initialization.
	 * When false, the environment may use its own seeding strategy.
	 */
	UPROPERTY(EditAnywhere, Category = "Environment Reset Options")
	bool bHasSeed = false;

};

