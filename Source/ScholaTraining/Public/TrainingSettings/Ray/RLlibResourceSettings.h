// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "TrainingSettings/TrainingSettings.h"
#include "TrainingUtils/ArgBuilder.h"
#include "RLlibResourceSettings.generated.h"

/**
 * @brief A struct to hold resource settings for an RLlib training script
 */
USTRUCT(BlueprintType)
struct SCHOLATRAINING_API FRLlibResourceSettings : public FTrainingSettings
{
	GENERATED_BODY()

public:
	
	/** The number of GPUs to use for training */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (InlineEditConditionToggle), Category = "Resource Settings")
	int NumGPUs = 0;

	/** Whether to use a custom number of CPUs for training */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resource Settings")
	bool bUseCustomNumberOfCPUs = false;

	/** The maximum number of CPUs/workers to use for training. If empty uses number of vCPUs on system */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bUseCustomNumberOfCPUs"), Category = "Resource Settings")
	int NumCPUs = 1;

	/** The number of learner processes to use for training. Set to 0 to run in main process */
	UPROPERTY(EditAnywhere, meta = (ClampMin = 0), BlueprintReadOnly, Category = "Resource Settings")
	int NumLearners = 0;

	/** The number of GPUs to use for the learner */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0), Category = "Resource Settings")
	int NumGPUsPerLearner = 0;

	/** The number of CPUs to use for the learner */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 1), Category = "Resource Settings")
	int NumCPUsPerLearner = 1;

	/** The number of CPUs to use for the main process */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 1), Category = "Resource Settings")
	int NumCPUsForMainProcess = 1;

	void GenerateTrainingArgs( FScriptArgBuilder& ArgBuilder) const;

	virtual ~FRLlibResourceSettings();
};