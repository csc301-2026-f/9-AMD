// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Spaces/Space.h"
#include "Points/Point.h"
#include "Common/LogSchola.h"
#include "ImitationAgentState.generated.h"

/**
 * @brief Struct representing imitation state received from external source.
 * @details Contains observations, actions, rewards, and completion flags for imitation learning.
 * This represents the full state for a single agent during one step of expert demonstration.
 */
USTRUCT(BlueprintType)
struct SCHOLAIMITATION_API FImitationAgentState
{
	GENERATED_BODY()

	/** Observations received by the agent in the current state. */
	UPROPERTY(BlueprintReadWrite, Category = "Schola|Imitation")
	TInstancedStruct<FPoint> Observations;

	/** Expert actions provided for the agent to follow. */
	UPROPERTY(BlueprintReadWrite, Category = "Schola|Imitation")
	TInstancedStruct<FPoint> Actions;

	/** Reward signal received for the current state. */
	UPROPERTY(BlueprintReadWrite, Category = "Schola|Imitation")
	float Reward = 0.0f;

	/** Flag indicating whether the episode has completed normally. */
	UPROPERTY(BlueprintReadWrite, Category = "Schola|Imitation")
	bool bTerminated = false;

	/** Flag indicating whether the episode was truncated (e.g., timeout). */
	UPROPERTY(BlueprintReadWrite, Category = "Schola|Imitation")
	bool bTruncated = false;

	/** Additional metadata as key-value pairs. */
	UPROPERTY(BlueprintReadWrite, Category = "Schola|Imitation")
	TMap<FString, FString> Info;

	/**
	 * @brief Reset all fields to their default values.
	 * @details Clears observations, actions, resets reward, completion flags, and empties info map.
	 */
    void Empty()
    {
		
		this->Observations.Reset();
        this->Actions.Reset();
        this->Reward = 0.0f;
        this->bTerminated = false;
        this->bTruncated = false;
        this->Info.Empty();
    }
};

