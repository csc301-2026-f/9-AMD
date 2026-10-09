// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Common/LogSchola.h"
#include "ImitationDataTypes/ImitationTrainingState.h"
#include "TrainingDataTypes/TrainingState.h"
#include "ImitationState.generated.h"

/**
 * @brief Struct combining both imitation state and initial state.
 * @details Encapsulates both the current training state (observations, actions, rewards) and
 * optional initial state information when environments reset. This is used for communication
 * between the connector and external training systems.
 */
USTRUCT(BlueprintType)
struct SCHOLAIMITATION_API FImitationState
{
	GENERATED_BODY()

	/** The training state containing observations, expert actions, and rewards for all environments. */
	UPROPERTY()
	FImitationTrainingState TrainingState;

	/** Optional initial state data populated when environments reset. */
	UPROPERTY()
	FInitialState InitialState;

	/** Flag indicating whether InitialState contains valid data. */
	UPROPERTY()
	bool bHasInitialState = false;
};

