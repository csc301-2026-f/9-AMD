// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "TrainingSettings/TrainingSettings.h"
#include "TrainingUtils/ArgBuilder.h"
#include "SB3CheckpointSettings.generated.h"

/**
 * @brief A struct to hold checkpoint settings for an SB3 training script
 */
USTRUCT(BlueprintType)
struct SCHOLATRAINING_API FSB3CheckpointSettings : public FTrainingSettings
{
	GENERATED_BODY()

public:
	/** Whether to save checkpoints during training */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Checkpoint Settings")
	bool bSaveCheckpoints = false;

	/** Whether to save the final model */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Checkpoint Settings")
	bool bSaveFinalModel = false;

	/** Whether to save the VecNormalize parameters */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bSaveCheckpoints || bSaveFinalModel"), Category = "Checkpoint Settings")
	bool bSaveVecNormalize = false;

	/** Whether to save the replay buffer */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bSaveCheckpoints"), Category = "Checkpoint Settings")
	bool bSaveReplayBuffer = false;

	/** Whether we should just create a checkpoint, or if we should also export the final model out to ONNX */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bSaveFinalModel"), Category = "Checkpoint Settings")
	bool bExportFinalModelToOnnx = true;

	/** The directory to save checkpoints to */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bSaveCheckpoints || bSaveFinalModel"), Category = "Checkpoint Settings")
	FDirectoryPath CheckpointDir;

	/** The frequency to save checkpoints */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bSaveCheckpoints"), Category = "Checkpoint Settings")
	int SaveFreq = 1000;

	/** The prefix to use for the checkpoint files */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bSaveCheckpoints || bSaveFinalModel"), Category = "Checkpoint Settings")
	FString NamePrefix = FString("ppo");

	void GenerateTrainingArgs(FScriptArgBuilder& ArgBuilder) const;

	virtual ~FSB3CheckpointSettings();
};
