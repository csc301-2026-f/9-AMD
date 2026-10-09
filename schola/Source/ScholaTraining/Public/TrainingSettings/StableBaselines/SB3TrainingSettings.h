// Copyright (c) 2023-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"

#include "TrainingSettings/TrainingSettings.h"
#include "TrainingSettings/StableBaselines/SB3LoggingSettings.h"
#include "TrainingSettings/StableBaselines/SB3CheckpointSettings.h"
#include "TrainingSettings/StableBaselines/SB3ResumeSettings.h"
#include "TrainingSettings/StableBaselines/SB3NetworkArchitectureSettings.h"
#include "TrainingSettings/StableBaselines/Algorithms/SB3PPOSettings.h"
#include "TrainingSettings/StableBaselines/Algorithms/SB3SACSettings.h"
#include "TrainingUtils/ArgBuilder.h"

#include "SB3TrainingSettings.generated.h"

/**
 * @brief Enumeration of reinforcement learning algorithms supported by Stable Baselines 3.
 */
UENUM()
enum class ESB3TrainingAlgorithm
{
	/** Proximal Policy Optimization. */
	PPO,
	/** Soft Actor-Critic. */
	SAC
};

/**
 * @brief Configuration struct for Stable Baselines 3 (SB3) training scripts.
 * @details Contains all settings necessary to launch and configure an SB3 training session.
 * This is a partial implementation of the available SB3 settings and is not exhaustive.
 */
USTRUCT(BlueprintType)
struct SCHOLATRAINING_API FSB3TrainingSettings : public FTrainingSettings
{
	GENERATED_BODY()

public:

	/** The total number of environment timesteps to train for. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0), Category = "Training Settings")
	int Timesteps = 8000;

	/** Settings for logging training metrics and outputs. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Training Settings")
	FSB3LoggingSettings LoggingSettings;

	/** Settings for model checkpointing during training. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Training Settings")
	FSB3CheckpointSettings CheckpointSettings;

	/** Settings for resuming training from a checkpoint. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Training Settings")
	FSB3ResumeSettings ResumeSettings;

	/** Settings for neural network architecture (e.g. layers, activation functions). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Training Settings")
	FSB3NetworkArchSettings NetworkArchitectureSettings;

	/** Whether to display a progress bar during training. Requires TQDM and Rich to be installed. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Training Settings")
	bool bDisplayProgressBar = true;

	/** When true, skips post-training evaluation (passes --disable-eval to the script). Useful to avoid hanging when training with few steps and deterministic eval. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Training Settings")
	bool bDisableEval = false;

	/** The reinforcement learning algorithm to use (e.g., SAC, PPO). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Training Settings")
	ESB3TrainingAlgorithm Algorithm = ESB3TrainingAlgorithm::PPO;

	/** PPO specific settings */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "Algorithm==ESB3TrainingAlgorithm::PPO", EditConditionHides, DisplayName = "PPO Algorithm Settings"), Category="Training Settings")
	FSB3PPOSettings PPOSettings;

	/** SAC specific settings */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "Algorithm==ESB3TrainingAlgorithm::SAC", EditConditionHides, DisplayName = "SAC Algorithm Settings"), Category="Training Settings")
	FSB3SACSettings SACSettings;

	/**
	 * @brief Generate command-line arguments for the SB3 training script.
	 * @param[in,out] ArgBuilder The argument builder to populate with settings.
	 */
	void GenerateTrainingArgs(FScriptArgBuilder& ArgBuilder) const;

	/**
	 * @brief Virtual destructor.
	 */
	virtual ~FSB3TrainingSettings();
};