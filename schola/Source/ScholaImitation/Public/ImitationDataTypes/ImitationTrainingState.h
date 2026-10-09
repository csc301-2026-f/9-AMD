// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Common/LogSchola.h"
#include "ImitationDataTypes/ImitationEnvironmentState.h"
#include "ImitationTrainingState.generated.h"

/**
 * @brief Struct representing the overall imitation training state.
 * @details Aggregates the states of all environments in a training session.
 * Each environment contains states for its agents, forming a hierarchical structure:
 * Training State -> Environment States -> Agent States.
 */
USTRUCT(BlueprintType)
struct SCHOLAIMITATION_API FImitationTrainingState
{
	GENERATED_BODY()

	/** Array of states for all environments in the training session. */
	UPROPERTY()
	TArray<FImitationEnvironmentState> EnvironmentStates;
};

