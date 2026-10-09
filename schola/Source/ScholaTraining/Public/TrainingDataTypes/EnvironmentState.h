// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Spaces/Space.h"
#include "Points/Point.h"
#include "Common/LogSchola.h"
#include "TrainingDataTypes/AgentState.h"
#include "EnvironmentState.generated.h"


/**
 * @brief An enumeration representing the various states common across gym and gym-like environments. Typically use Running, Truncated and Completed
 */
UENUM(BlueprintType)
enum class EAgentTrainingStatus : uint8
{
	Running	  UMETA(DisplayName = "Running"),
	Truncated UMETA(DisplayName = "Truncated"),
	Completed UMETA(DisplayName = "Completed"),
};

/**
 * @brief Enum class representing the status of the environment.
 */
UENUM(BlueprintType)
enum class EEnvironmentStatus : uint8
{
	Active	  UMETA(DisplayName = "Active"),
	Completed UMETA(DisplayName = "Completed"),
};


/**
 * @brief Environment state after a reset operation.
 * @details Contains initial states for all agents in the environment.
 */
USTRUCT(BlueprintType)
struct SCHOLATRAINING_API FInitialEnvironmentState
{
	GENERATED_BODY()

	/** Map of agent IDs to their initial states. */
	UPROPERTY()
	TMap<FString, FInitialAgentState> AgentStates;

	/**
	 * @brief Default constructor.
	 */
	FInitialEnvironmentState() {};

};



/**
 * @brief Environment state during training.
 * @details Contains states for all agents and the overall environment status.
 */
USTRUCT(BlueprintType)
struct SCHOLATRAINING_API FEnvironmentState
{
	GENERATED_BODY()

	/** Map of agent IDs to their current states. */
	UPROPERTY()
	TMap<FString, FAgentState> AgentStates;

	/** The current status of the environment (e.g. Active or Completed). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reinforcement Learning")
	EEnvironmentStatus EnvironmentStatus = EEnvironmentStatus::Active;

	/**
	 * @brief Mark the environment as active.
	 */
	void MarkActive()
	{
		this->EnvironmentStatus = EEnvironmentStatus::Active;
	}

	/**
	 * @brief Mark the environment as completed.
	 */
	void MarkCompleted()
	{
		this->EnvironmentStatus = EEnvironmentStatus::Completed;
	}

	/**
	 * @brief Check if the environment is currently active.
	 * @return True if the environment is active.
	 */
	bool IsEnvironmentActive()
	{
		return this->EnvironmentStatus == EEnvironmentStatus::Active;
	}

	/**
	 * @brief Check if the environment has completed.
	 * @return True if the environment is completed.
	 */
	bool IsEnvironmentCompleted()
	{
		return this->EnvironmentStatus == EEnvironmentStatus::Completed;
	}

	/**
	 * @brief Default constructor.
	 */
	FEnvironmentState() {};
};