// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "TrainingUtils/ArgBuilder.h"
#include "TrainingSettings/TrainingSettings.h"
#include "RLlibCheckpointSettings.generated.h"

/**
 * @brief A struct to hold checkpoint settings for an RLlib training script
 */
USTRUCT(BlueprintType)
struct SCHOLATRAINING_API FRLlibCheckpointSettings : public FTrainingSettings
{
	GENERATED_BODY()

public:
	/** Whether to save the final output of training */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Checkpoint Settings")
	bool bSaveFinalModel = true;

	/** Whether to export the final model to ONNX automatically */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bSaveFinalModel"), Category = "Checkpoint Settings")
	bool bExportToONNX = false;

	/** Whether to save checkpoints during training */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Checkpoint Settings")
	bool bEnableCheckpoints = false;

	/** How frequently should we make checkpoints */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bEnableCheckpoints"), Category = "Checkpoint Settings")
	int SaveFreq = 1000;

	/** The directory to save checkpoints to */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Checkpoint Settings")
	FDirectoryPath CheckpointDir;

	void GenerateTrainingArgs( FScriptArgBuilder& ArgBuilder) const;

	virtual ~FRLlibCheckpointSettings();
};
