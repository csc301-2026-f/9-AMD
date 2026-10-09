// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"

#include "TrainingUtils/ArgBuilder.h"
#include "TrainingSettings/TrainingSettings.h"
#include "RLlibAPPOSettings.generated.h"

/**
 * @brief A struct to hold Asynchronous Proximal Policy Optimization(APPO) settings for an RLLib training script
 * @note This is a partial implementation of the APPO settings, and is not exhaustive
 */
USTRUCT(BlueprintType)
struct SCHOLATRAINING_API FRLlibAPPOSettings : public FTrainingSettings
{
	GENERATED_BODY()

public:
	/** Whether to use V-trace for off-policy correction (APPO). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "APPO Settings")
    bool bVTrace = true;

    /** V-trace rho clipping threshold. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "APPO Settings")
    float VTraceClipRhoThreshold = 1.0;

    /** V-trace policy-gradient rho clipping threshold. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "APPO Settings")
    float VTraceClipPGRhoThreshold = 1.0;

    /** GAE lambda for advantage estimation. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "APPO Settings")
    float GAELambda = 0.95;

    /** PPO-style policy clipping parameter. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "APPO Settings")
    float ClipParam = 0.2;

    /** Whether to use generalized advantage estimation. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "APPO Settings")
    bool bUseGAE = true;

    /** Appends APPO-related CLI arguments to the training script builder. */
    void GenerateTrainingArgs(FScriptArgBuilder& ArgBuilder) const;

	virtual ~FRLlibAPPOSettings();
};
