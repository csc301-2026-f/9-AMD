// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "TrainingSettings/TrainingSettings.h"
#include "TrainingUtils/ArgBuilder.h"
#include "SB3LoggingSettings.generated.h"

/**
 * @brief A struct to hold logging settings for an SB3 training script
 */
USTRUCT(BlueprintType)
struct SCHOLATRAINING_API FSB3LoggingSettings : public FTrainingSettings
{
	GENERATED_BODY()

public:
	/** The verbosity of the environment logging */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0", ClampMax = "2"), Category = "Logging Settings|Environment")
	int32 EnvLoggingVerbosity = 0;

	/** The verbosity of the trainer logging */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0", ClampMax = "2"), Category = "Logging Settings|Trainer")
	int32 TrainerLoggingVerbosity = 0;

	/* Logging related arguments */

	/** Whether to save training logs with tensorboard */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Logging Settings|Tensorboard")
	bool bSaveTBLogs = false;

	/** The frequency to save tensorboard logs */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bSaveTBLogs"), Category = "Logging Settings|Tensorboard")
	int LogFreq = 10;

	/** The directory to save logs to */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bSaveTBLogs"), Category = "Logging Settings|Tensorboard")
	FDirectoryPath LogDir;

	/** The verbosity of the callback logging */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bSaveTBLogs", ClampMin = "0", ClampMax = "2"), Category = "Logging Settings|Callback")
	int CallbackVerbosity = 1;

	void GenerateTrainingArgs(FScriptArgBuilder& ArgBuilder) const;

	virtual ~FSB3LoggingSettings();
};
