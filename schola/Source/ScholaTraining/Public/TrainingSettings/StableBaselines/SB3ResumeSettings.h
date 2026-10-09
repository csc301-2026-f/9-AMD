// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "TrainingSettings/TrainingSettings.h"
#include "TrainingUtils/ArgBuilder.h"
#include "SB3ResumeSettings.generated.h"

/**
 * @brief A struct to hold resume settings for an SB3 training script
 */
USTRUCT(BlueprintType)
struct SCHOLATRAINING_API FSB3ResumeSettings
{
	GENERATED_BODY()

public:
	/** Whether to load a model from a file */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (InlineEditConditionToggle), Category = "Resume Settings")
	bool bLoadModel = false;

	/** The path to the model to load, if we are loading a model */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bLoadModel", FilePathFilter = "zip", DisplayName = "Resume From Policy Saved to:"), Category = "Resume Settings")
	FFilePath ModelPath;

	/** Whether to load a replay buffer from a file */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (InlineEditConditionToggle), Category = "Resume Settings")
	bool bLoadReplayBuffer = false;

	/** The path to the replay buffer to load, if we are loading a replay buffer */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bLoadReplayBuffer", FilePathFilter = "pkl", DisplayName = "Load Replay Buffer Saved to:"), Category = "Resume Settings")
	FFilePath ReplayBufferPath;

	/** Whether to load VecNormalize parameters from a file */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (InlineEditConditionToggle), Category = "Resume Settings")
	bool bLoadVecNormalize = false;

	/** The path to the VecNormalize parameters to load, if we are loading them */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bLoadVecNormalize", FilePathFilter = "pkl", DisplayName = "Load VecNormalize Parameters Saved to:"), Category = "Resume Settings")
	FFilePath VecNormalizePath;

	/** When resuming, reset the internal timestep counter to 0 (passes --reset-timestep to the script). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resume Settings")
	bool bResetTimestep = false;

	/** Appends resume-related arguments for Stable Baselines 3 training scripts. */
	void GenerateTrainingArgs(FScriptArgBuilder& ArgBuilder) const;

	virtual ~FSB3ResumeSettings();
};
