// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Common/LogSchola.h"
#include "TrainingDataTypes/EnvironmentState.h"
#include "TrainingState.generated.h"

/**
 * @brief A Struct representing the state of the training session given by a set of EnvironmentStates.
 * @details This structure contains the current state of all environments during training,
 * including observations, rewards, and completion status for each agent in each environment.
 */
USTRUCT(BlueprintType)
struct SCHOLATRAINING_API FTrainingState
{
	GENERATED_BODY()

	/** Array of environment states indexed by environment ID. */
	TArray<FEnvironmentState> EnvironmentStates;

	/**
	 * @brief Default constructor.
	 */
	FTrainingState(){};

};

/**
 * @brief A struct representing the initial state of environments after a reset operation.
 * @details This structure contains the initial observations and info dictionaries for all
 * environments that have been reset, typically returned after calling reset() on environments.
 */
USTRUCT(BlueprintType)
struct SCHOLATRAINING_API FInitialState
{
	GENERATED_BODY()

	/** Map from environment ID to initial environment state containing initial observations. */
	TMap<int, FInitialEnvironmentState> EnvironmentStates;

	/**
	 * @brief Default constructor.
	 */
	FInitialState() {};
};


