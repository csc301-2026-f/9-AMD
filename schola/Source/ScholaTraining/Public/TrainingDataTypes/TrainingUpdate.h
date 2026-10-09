// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Containers/SortedMap.h"
#include "TrainingDataTypes/EnvironmentUpdate.h"
#include "TrainingUpdate.generated.h"

/**
 * @brief An enumeration representing the type of action taken by an agent.
 */
UENUM(BlueprintType)
enum class EConnectorStatusUpdate : uint8
{
	NONE = 0 UMETA(DisplayName = "No New Status"),
	ERRORED = 1 UMETA(DisplayName = "Error"),
	CLOSED = 2 UMETA(DisplayName = "Closed"),
};

/**
 * @brief A training step update containing actions for all environments.
 * @details Used when the training system wants environments to execute actions.
 */
USTRUCT(BlueprintType)
struct SCHOLATRAINING_API FTrainingStep
{
	GENERATED_BODY()
	/** Array of environment steps, one per environment. */
	TArray<FEnvStep> EnvSteps;
	
	/**
	 * @brief Construct a new empty training step.
	 */
	FTrainingStep()
	{
	}

	/**
	 * @brief Construct a training step with environment steps.
	 * @param[in] InEnvSteps Array of steps to execute.
	 */
	FTrainingStep(const TArray<FEnvStep>& InEnvSteps) : EnvSteps(InEnvSteps)
	{
	}

};

/**
 * @brief A training reset update specifying which environments to reset.
 * @details Used when the training system wants to reset one or more environments.
 */
USTRUCT(BlueprintType)
struct SCHOLATRAINING_API FTrainingReset
{
	GENERATED_BODY()
	/** Map from environment ID to reset configuration (seed, options). */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Schola|Training")
	TMap<int, FEnvReset> Environments;
	
	/**
	 * @brief Construct a new empty training reset.
	 */
	FTrainingReset()
	{

	}
};

/**
 * @brief An enumeration representing the type of update.
 */
UENUM(BlueprintType)
enum class ETrainingUpdateType : uint8
{
	NONE = 0 UMETA(DisplayName = "None"),
	STEP = 1 UMETA(DisplayName = "Step"),
	RESET = 2 UMETA(DisplayName = "Reset"),
};

/**
 * @brief A Struct representing an update to the overall training state.
 */
USTRUCT(BlueprintType)
struct SCHOLATRAINING_API FTrainingStateUpdate
{
	GENERATED_BODY()

	/** The type of update */
	UPROPERTY()
	ETrainingUpdateType UpdateType = ETrainingUpdateType::NONE;

	/** Training step data (valid when UpdateType == STEP) */
	UPROPERTY()
	FTrainingStep StepData;

	/** Training reset data (valid when UpdateType == RESET) */
	UPROPERTY()
	FTrainingReset ResetData;

	/** The status of the connector */
	UPROPERTY()
	EConnectorStatusUpdate Status = EConnectorStatusUpdate::NONE;

	/**
	 * @brief Construct a new empty training state update.
	 */
	FTrainingStateUpdate()
	{

	}

	/**
	 * @brief Get the step data from this update.
	 * @return Reference to the step data.
	 */
	const FTrainingStep& GetStep() const
	{
		return StepData;
	}

	/**
	 * @brief Get the reset data from this update.
	 * @return Reference to the reset data.
	 */
	const FTrainingReset& GetReset() const
	{
		return ResetData;
	}

	/**
	 * @brief Check if this is a step update.
	 * @return True if this update contains a step.
	 */
	const bool IsStep() const
	{
		return UpdateType == ETrainingUpdateType::STEP && this->Status == EConnectorStatusUpdate::NONE;
	}

	/**
	 * @brief Check if this is a reset update.
	 * @return True if this update contains a reset.
	 */
	const bool IsReset() const
	{
		return UpdateType == ETrainingUpdateType::RESET && this->Status == EConnectorStatusUpdate::NONE;
	}

	/**
	 * @brief Check if the connector has encountered an error.
	 * @return True if the status indicates an error.
	 */
	const bool IsError() const
	{
		return Status == EConnectorStatusUpdate::ERRORED;
	}

	/**
	 * @brief Check if the connector has been closed.
	 * @return True if the status indicates closure.
	 */
	const bool IsClosed() const
	{
		return Status == EConnectorStatusUpdate::CLOSED;
	}

};
