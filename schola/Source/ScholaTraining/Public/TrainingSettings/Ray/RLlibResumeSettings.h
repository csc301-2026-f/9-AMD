// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "TrainingSettings/TrainingSettings.h"
#include "TrainingUtils/ArgBuilder.h"
#include "RLlibResumeSettings.generated.h"

/**
 * @brief A struct to hold resume settings for an RLlib training script
 */
USTRUCT(BlueprintType)
struct SCHOLATRAINING_API FRLlibResumeSettings
{
	GENERATED_BODY()

public:
	/** Whether to load a model from a file */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (InlineEditConditionToggle), Category = "Resume Settings")
	bool bLoadModel = false;

	/** The path to the model to load, if we are loading a model */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bLoadModel", DisplayName = "Resume From Policy Saved to:"), Category = "Resume Settings")
	FFilePath ModelPath;

	/** Appends resume / checkpoint arguments for RLlib training scripts. */
	void GenerateTrainingArgs( FScriptArgBuilder& ArgBuilder) const;

	virtual ~FRLlibResumeSettings();
};
