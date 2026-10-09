// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Common/LogSchola.h"
#include "ImitationDataTypes/ImitationAgentState.h"
#include "ImitationEnvironmentState.generated.h"

/**
 * @brief Struct representing the imitation state for an environment.
 * @details Aggregates the imitation states of all agents within a single environment instance.
 * This allows multi-agent environments to report states for each of their agents.
 */
USTRUCT(BlueprintType)
struct SCHOLAIMITATION_API FImitationEnvironmentState
{
	GENERATED_BODY()

	/** Map from agent identifiers to their respective imitation states. */
	UPROPERTY()
	TMap<FString, FImitationAgentState> AgentStates;
};

