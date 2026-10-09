// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Spaces/Space.h"
#include "Points/Point.h"
#include "Common/LogSchola.h"
#include "AgentState.generated.h"

/**
 * @brief Agent state returned after environment reset.
 * @details Contains the initial observation and optional info dictionary.
 */
USTRUCT(BlueprintType)
struct SCHOLATRAINING_API FInitialAgentState
{
	GENERATED_BODY()

	/** The initial observation from the agent's observation space. */
	UPROPERTY(BlueprintReadWrite, Category = "Schola|Training")
	TInstancedStruct<FPoint> Observations;

	/** Optional info dictionary with additional environment information. */
	UPROPERTY(BlueprintReadWrite, Category = "Schola|Training")
	TMap<FString, FString> Info;

	/**
	 * @brief Constructor with observation and info.
	 * @param[in] InPoint The initial observation.
	 * @param[in] InInfo The info dictionary.
	 */
	FInitialAgentState(const TInstancedStruct<FPoint>& InPoint, const TMap<FString, FString>& InInfo)
		: Observations(InPoint), Info(InInfo) {}

	/**
	 * @brief Default constructor.
	 */
	FInitialAgentState() {};
};


/**
 * @brief Agent state returned after an environment step.
 * @details Extends FInitialAgentState with reward, done, and truncated flags following the gym convention.
 */
USTRUCT(BlueprintType)
struct SCHOLATRAINING_API FAgentState : public FInitialAgentState
{
	GENERATED_BODY()

	/** The scalar reward received after taking the action. */
	UPROPERTY(BlueprintReadWrite, Category = "Schola|Training")
	float Reward = 0.0f;

	/** Whether the episode has ended due to task completion. */
	UPROPERTY(BlueprintReadWrite, Category = "Schola|Training")
	bool bTerminated = false;

	/** Whether the episode was truncated (e.g., time limit reached). */
	UPROPERTY(BlueprintReadWrite, Category = "Schola|Training")
	bool bTruncated = false;

	/**
	 * @brief Constructor with full state information.
	 * @param[in] InPoint The observation.
	 * @param[in] Reward The reward value.
	 * @param[in] bTerminated Whether the episode is complete.
	 * @param[in] bTruncated Whether the episode was truncated.
	 * @param[in] InInfo The info dictionary.
	 */
	FAgentState(const TInstancedStruct<FPoint>& InPoint, float Reward, bool bTerminated,bool bTruncated, const TMap<FString, FString>& InInfo)
		: FInitialAgentState(InPoint, InInfo), Reward(Reward), bTerminated(bTerminated), bTruncated(bTruncated) {}

	/**
	 * @brief Default constructor.
	 */
	FAgentState() {};
};
