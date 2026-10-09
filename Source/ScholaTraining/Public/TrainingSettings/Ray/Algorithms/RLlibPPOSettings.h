// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"

#include "TrainingUtils/ArgBuilder.h"
#include "TrainingSettings/TrainingSettings.h"
#include "RLlibPPOSettings.generated.h"

/**
 * @brief A struct to hold Proximal Policy Optimization(PPO) settings for an RLLib training script
 * @note This is a partial implementation of the PPO settings, and is not exhaustive
 */
USTRUCT(BlueprintType)
struct SCHOLATRAINING_API FRLlibPPOSettings : public FTrainingSettings
{
	GENERATED_BODY()

public:
	/** GAE lambda for advantage estimation. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PPO Settings")
    float GAELambda = 0.95;

    /** PPO surrogate clipping range. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PPO Settings")
    float ClipParam = 0.2;

    /** Whether to use generalized advantage estimation. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PPO Settings")
    bool bUseGAE = true;

	/** Appends PPO-related CLI arguments to the training script builder. */
	void GenerateTrainingArgs( FScriptArgBuilder& ArgBuilder) const;

	virtual ~FRLlibPPOSettings();
};
